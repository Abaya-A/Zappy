import os
import random
import yaml
import numpy as np
import torch
import torch.nn as nn
import torch.optim as optim
import torch.nn.functional as F
import torch.autograd as autograd
import gymnasium as gym
from torch.autograd import Variable
from collections import deque, namedtuple

class ANN(nn.Module):

    def __init__(self, state_size, action_size, seed=42):
        super(ANN, self).__init__()
        self.seed = torch.manual_seed(seed)
        self.fc1 = nn.Linear(state_size, 64)
        self.fc2 = nn.Linear(64, 64)
        self.fc3 = nn.Linear(64, action_size)

    def forward(self, state):
        x = self.fc1(state)
        x = F.relu(x)
        x = self.fc2(x)
        x = F.relu(x)
        return self.fc3(x)

class ReplayMemory(object):

    def __init__(self, capacity):
        self.capacity = capacity
        self.memory = []

    def push(self, event):
        self.memory.append(event)
        if len(self.memory) > self.capacity:
            del self.memory[0]

    def sample(self, batch_size):
        experiences = random.sample(self.memory, batch_size)
        states = torch.from_numpy(np.vstack([e[0] for e in experiences if e is not None])).float()
        actions = torch.from_numpy(np.vstack([e[1] for e in experiences if e is not None])).long()
        rewards = torch.from_numpy(np.vstack([e[2] for e in experiences if e is not None])).float()
        next_states = torch.from_numpy(np.vstack([e[3] for e in experiences if e is not None])).float()
        dones = torch.from_numpy(np.vstack([e[4] for e in experiences if e is not None]).astype(np.uint8)).float()

        return states, actions, rewards, next_states, dones

class Agent():

    def __init__(self, state_size, action_size, env, cfg=None, use_conv=True):
        if cfg is None:
            cfg = {}
        self.learning_rate = float(cfg.get('learning_rate', 5e-4))
        self.minibatch = int(cfg.get('minibatch', 64))
        self.gamma = float(cfg.get('gamma', 0.99))
        self.interpolation_parameter = float(cfg.get('interpolation_parameter', 1e-3))
        self.replay_buffer_size = int(cfg.get('replay_buffer_size', 100000))
        self.state_size = state_size
        self.action_size = action_size
        self.use_conv = use_conv

        self.model1 = ANN(state_size, action_size)
        self.model2 = ANN(state_size, action_size)
        self.optimizer1 = torch.optim.Adam(self.model1.parameter(), lr=self.learning_rate)
        self.optimizer2 = torch.optim.Adam(self.model2.parameter(), lr=self.learning_rate)

    def step(self, state, action, reward, next_state, done):
        self.memory.push((state, action, reward, next_state, done))
        self.t_step = (self.t_step + 1) % 4
        if self.t_step == 0 and len(self.memory.memory) > self.minibatch:
            experiences = self.memory.sample(self.minibatch)
            self.compute_loss(experiences)

    def get_action(self, state, epsilon):
        state = torch.from_numpy(state).float().unsqueeze(0)
        self.local_qnetwork.eval()
        with torch.no_grad():
            action_values = self.local_qnetwork(state)
        self.local_qnetwork.train()
        if random.random() > epsilon:
            return np.argmax(action_values.cpu().data.numpy())
        else:
            return random.choice(np.arange(self.action_size))

    def compute_loss(self, experiences):
        states, actions, rewards, next_states, dones = experiences
        states = torch.FloatTensor(states).to(self.device)
        actions = torch.LongTensor(actions).to(self.device)
        rewards = torch.FloatTensor(rewards).to(self.device)
        next_states = torch.FloatTensor(next_states).to(self.device)
        dones = torch.FloatTensor(dones)

        actions = actions.view(actions.size(0), 1)
        dones = dones.view(dones.size(0), 1)

        curr_q1 = self.model1.forward(states).gather(1, actions)
        curr_q2 = self.model2.forward(states).gather(1, actions)

        next_q1 = self.model1.forward(next_states)
        next_q2 = self.model2.forward(next_states)
        next_q = torch.min(
            torch.max(self.model1.forward(next_states), 1)[0],
            torch.max(self.model2.forward(next_states), 1)[0]
        )
        next_q = next_q.view(next_q.size(0), 1)
        expected_q = rewards + (1 - dones) * self.gamma * next_q

        loss1 = F.mse_loss(curr_q1, expected_q.detach())
        loss2 = F.mse_loss(curr_q2, expected_q.detach())
        return loss1, loss2

    def update(self, batch_size):
        experiences = self.replay_buffer.sample(batch_size)
        loss1, loss2 = self.compute_loss(experiences)
        self.optimizer1.zero_grad()
        loss1.backward()
        self.optimizer1.step()
        self.optimizer2.zero_grad()
        loss2.backward()
        self.optimizer2.step()