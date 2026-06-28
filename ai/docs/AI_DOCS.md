# Documentation de l’IA Zappy

## 1. Objectif de l’IA

Cette IA contrôle automatiquement un joueur dans le jeu **Zappy**.

Son but est de survivre, collecter les ressources nécessaires, communiquer avec les autres joueurs de son équipe, se regrouper quand c’est nécessaire, puis lancer des incantations pour monter de niveau.

L’IA fonctionne en boucle avec trois étapes principales :

```text
observe -> think -> act
```

- `observe` : lire l’état actuel du monde.
- `think` : choisir quoi faire ensuite.
- `act` : exécuter l’action choisie.

Cette séparation permet de garder une logique claire : l’IA ne décide pas pendant qu’elle lit les données, et elle n’exécute pas d’action sans avoir d’abord choisi un objectif.

---

## 2. Architecture globale

L’IA est divisée en plusieurs parties :

```text
Client réseau
    ↓
Agent
    ↓
State
    ↓
Parser / Vision / Levels
```

### Client réseau

Le client gère la communication avec le serveur Zappy.

Il envoie les commandes :

```text
Forward
Left
Right
Look
Inventory
Take linemate
Set linemate
Broadcast LEVELUP:2
Incantation
```

Il lit aussi les réponses du serveur.

Le point important est que le serveur peut envoyer des messages asynchrones à n’importe quel moment :

```text
message 4, LEVELUP:2
Elevation underway
Current level: 3
eject: 5
```

Le client doit donc nettoyer ce flux pour éviter que l’agent lise une mauvaise réponse.

### Agent

L’agent contient la logique de décision.

Il décide si le joueur doit :

- chercher de la nourriture ;
- chercher une ressource ;
- suivre un broadcast ;
- appeler d’autres joueurs ;
- lancer une incantation ;
- fork si possible.

### State

Le state contient la mémoire actuelle du joueur :

- son niveau ;
- son inventaire ;
- ce qu’il voit autour de lui ;
- les messages reçus ;
- son objectif actuel ;
- son rôle ;
- les cooldowns.

### Parser

Le parser transforme les réponses texte du serveur en données utilisables.

Exemple :

```text
[ food 10, linemate 1, deraumere 0 ]
```

devient :

```python
{
    "food": 10,
    "linemate": 1,
    "deraumere": 0
}
```

Et :

```text
message 4, LEVELUP:2
```

devient :

```python
direction = 4
content = "LEVELUP:2"
```

### Vision

La partie vision sert à analyser le résultat de `Look`.

Exemple :

```text
[ player food, linemate, deraumere ]
```

devient une liste de cases visibles :

```python
[
    ["player", "food"],
    ["linemate"],
    ["deraumere"]
]
```

### Levels

Le fichier des niveaux contient les ressources nécessaires pour chaque incantation.

Exemple :

```python
LEVELS = {
    1: {
        "players": 1,
        "linemate": 1
    },
    2: {
        "players": 2,
        "linemate": 1,
        "deraumere": 1,
        "sibur": 1
    }
}
```

---

## 3. Fonctionnement du client

Le client est responsable de la communication avec le serveur.

### Connexion

Au lancement, le client se connecte au serveur TCP.

Le serveur envoie :

```text
WELCOME
```

Le client répond avec le nom de l’équipe :

```text
team1
```

Puis le serveur répond :

```text
nombre_de_slots
largeur hauteur
```

Exemple :

```text
5
10 10
```

Cela signifie que le joueur est connecté, qu’il reste 5 slots disponibles, et que la map fait 10x10.

---

## 4. Commandes synchrones et messages asynchrones

Le protocole Zappy n’envoie pas uniquement des réponses directes aux commandes.

Par exemple, si l’IA envoie :

```text
Inventory
```

elle s’attend à recevoir :

```text
[ food 10, linemate 1, ... ]
```

Mais le serveur peut envoyer avant cela :

```text
message 4, LEVELUP:2
```

Donc le client doit ignorer ou stocker les messages asynchrones, puis continuer à attendre la vraie réponse de `Inventory`.

### Exemple de flux normal

```text
AI -> Inventory
SERVER -> [ food 10, linemate 1 ]
```

### Exemple de flux avec broadcast

```text
AI -> Inventory
SERVER -> message 4, LEVELUP:2
SERVER -> [ food 10, linemate 1 ]
```

Le client doit stocker le message dans `self.messages`, puis retourner uniquement l’inventaire à l’agent.

---

## 5. Gestion des incantations asynchrones

Une incantation peut démarrer alors qu’un joueur est en train de faire autre chose.

Exemple :

```text
AI -> Forward
SERVER -> message 0, LEVELUP:1
SERVER -> Elevation underway
SERVER -> Current level: 2
SERVER -> ok
```

Dans ce cas :

- `message 0, LEVELUP:1` est un broadcast ;
- `Elevation underway` signifie qu’une incantation a commencé sur la case du joueur ;
- `Current level: 2` signifie que le joueur a monté de niveau ;
- `ok` est la vraie réponse à la commande `Forward`.

Le client doit donc :

1. stocker le broadcast ;
2. lire le résultat de l’incantation ;
3. stocker `Current level: 2` dans `current_level_line` ;
4. continuer à lire jusqu’à la vraie réponse de la commande de départ.

C’est ce qui évite les bugs du type :

```text
Inventory -> ko
Look -> [ food 7, ... ]
Inventory: {}
Vision: [['food', '7']]
```

Ce bug arrive quand une réponse reste dans le buffer et est lue par la mauvaise commande.

---

## 6. Boucle principale de l’agent

L’agent tourne en boucle :

```python
while True:
    self.observe()
    self.think()
    self.act()
```

### Étape 1 : observe

`observe()` met à jour la perception du joueur.

Il fait :

1. synchronisation du niveau si le client a reçu `Current level: X` ;
2. commande `Inventory` ;
3. parsing de l’inventaire ;
4. commande `Look` ;
5. parsing de la vision ;
6. récupération des messages stockés par le client.

Exemple :

```python
raw_inventory = self.client.command("Inventory")
self.state.inventory = parse_inventory(raw_inventory)

raw_look = self.client.command("Look")
self.state.visible_tiles = parse_look(raw_look)

self.state.messages.extend(self.client.messages)
self.client.messages.clear()
```

À la fin de `observe`, l’agent sait :

- combien il a de nourriture ;
- quelles ressources il possède ;
- ce qu’il voit autour de lui ;
- quels broadcasts il a reçus.

---

## 7. Étape think

`think()` choisit le prochain objectif.

L’ordre de priorité est important.

### Priorité 1 : suivre les broadcasts utiles

Si l’agent reçoit :

```text
message 4, LEVELUP:2
```

et qu’il est lui-même niveau 2, alors il doit suivre la direction 4.

Si le message concerne un autre niveau, il l’ignore.

Exemple :

```text
Bot niveau 1 reçoit LEVELUP:2 -> ignore
Bot niveau 2 reçoit LEVELUP:2 -> suit
```

### Priorité 2 : monter de niveau si possible

Si le joueur a les ressources nécessaires et le bon nombre de joueurs sur la case, il choisit :

```python
current_goal = "LEVEL_UP"
```

### Priorité 3 : chercher de la nourriture

Si le joueur a moins de 10 food, il cherche de la nourriture.

```python
if self.state.food() < 10:
    self.state.current_goal = "SEARCH_FOOD"
```

La nourriture est prioritaire parce que sans food le joueur meurt.

### Priorité 4 : chercher les ressources manquantes

Si le joueur n’a pas encore les ressources nécessaires à son niveau, il cherche la prochaine ressource manquante.

Exemple niveau 2 :

```text
Besoin : linemate, deraumere, sibur
Inventaire : linemate 1, deraumere 0, sibur 1
Objectif : chercher deraumere
```

### Priorité 5 : appeler les autres joueurs

Si toutes les ressources sont prêtes mais qu’il manque des joueurs, l’agent choisit :

```python
current_goal = "CALL_PLAYERS"
```

Il va ensuite broadcast :

```text
Broadcast LEVELUP:2
```

---

## 8. Étape act

`act()` exécute l’objectif choisi dans `think()`.

Exemples :

```python
if goal == "FOLLOW_BROADCAST":
    self.follow_broadcast()

if goal == "LEVEL_UP":
    self.level_up()

if goal == "CALL_PLAYERS":
    self.call_players()

if goal.startswith("SEARCH_"):
    self.search_resource(resource)
```

L’agent ne réfléchit pas dans `act()`. Il exécute seulement l’objectif.

---

## 9. Recherche de ressources

Pour chercher une ressource, l’agent regarde d’abord sa case actuelle.

Si la ressource est déjà sur la case :

```text
Take linemate
```

Sinon, il cherche dans les cases visibles.

Exemple :

```python
tile_index = self.state.find_resource("linemate")
```

S’il trouve la ressource dans la vision, il se déplace vers cette case.

S’il ne voit pas la ressource, il avance :

```text
Forward
```

---

## 10. Vision et déplacement

Le serveur renvoie les cases visibles sous forme de liste.

Exemple :

```text
[ player, food, linemate, deraumere ]
```

Chaque index correspond à une case autour du joueur.

La fonction `tile_to_position(tile_index)` transforme l’index d’une case en coordonnées relatives :

```python
x, y = tile_to_position(tile_index)
```

- `y` représente la profondeur devant le joueur ;
- `x` représente le décalage gauche/droite.

Ensuite, `move_to_tile()` transforme ces coordonnées en commandes :

```text
Left
Forward
Right
Forward
```

ou :

```text
Right
Forward
Left
Forward
```

selon la position de la case.

Le but est d’avancer jusqu’à la case qui contient la ressource ciblée.

---

## 11. Collecte sur la case actuelle

Si l’agent n’a pas d’objectif spécifique mais qu’il voit des ressources sur sa case, il peut les prendre.

```python
def collect_current_tile(self):
    current_tile = self.state.current_tile()

    for resource in current_tile:
        if resource not in RESOURCE:
            continue

        self.client.command(f"Take {resource}")
```

Cela permet de récupérer automatiquement les ressources utiles rencontrées.

---

## 12. Gestion de la nourriture

La nourriture est la condition de survie.

L’agent surveille son inventaire :

```python
self.state.food()
```

Si la quantité de nourriture descend sous un seuil, par exemple 10, l’objectif devient :

```python
SEARCH_FOOD
```

Cela évite que l’agent continue à chercher des pierres alors qu’il est proche de mourir.

---

## 13. Gestion des niveaux

Chaque niveau demande :

- un nombre précis de joueurs ;
- une combinaison précise de ressources ;
- les ressources posées sur la case ;
- les joueurs présents sur la même case.

Exemple niveau 1 :

```text
1 joueur
1 linemate
```

Exemple niveau 2 :

```text
2 joueurs
1 linemate
1 deraumere
1 sibur
```

L’agent vérifie les conditions avec :

```python
self.state.ready_for_incantation()
```

Cette fonction doit vérifier :

1. le bon nombre de joueurs ;
2. les ressources nécessaires dans l’inventaire ;
3. pas simplement “au moins” le nombre de joueurs, mais exactement le nombre attendu.

---

## 14. Nettoyage de la case avant incantation

Avant de poser les pierres, l’agent nettoie la case.

Le but est d’éviter d’avoir trop de ressources sur la case.

Exemple de bug :

```text
La case contient déjà linemate
L’agent pose encore linemate
La case contient 2 linemate
L’incantation échoue
```

Donc avant de poser les ressources nécessaires, l’agent prend les pierres déjà présentes :

```python
INCANTATION_RESOURCES = [
    "linemate",
    "deraumere",
    "sibur",
    "mendiane",
    "phiras",
    "thystame",
]
```

Puis :

```python
for item in current_tile:
    if item in INCANTATION_RESOURCES:
        self.client.command(f"Take {item}")
```

Important : on ne nettoie pas forcément la food. La food n’est pas une ressource d’incantation.

---

## 15. Déroulement d’une incantation simple

Pour lancer une incantation, l’agent fait :

1. vérifie le nombre de joueurs ;
2. vérifie les ressources ;
3. nettoie la case ;
4. pose les ressources nécessaires ;
5. broadcast pour informer les autres ;
6. envoie `Incantation` ;
7. lit le résultat ;
8. met à jour son niveau.

Exemple :

```text
Set linemate
Broadcast LEVELUP:1
Incantation
Elevation underway
Current level: 2
```

Quand le client reçoit :

```text
Current level: 2
```

l’agent met à jour :

```python
self.state.level = 2
```

---

## 16. Broadcasts entre joueurs

Les joueurs communiquent avec :

```text
Broadcast LEVELUP:<level>
```

Exemple :

```text
Broadcast LEVELUP:2
```

Cela veut dire :

> “Je suis en train de préparer une incantation pour passer du niveau 2 au niveau 3. Si tu es niveau 2, viens.”

Les autres joueurs reçoivent :

```text
message 4, LEVELUP:2
```

Le `4` est la direction du son.

L’agent parse ce message :

```python
direction, content = parse_broadcast(msg)
```

Puis il vérifie :

```python
content == "LEVELUP:2"
```

Si le niveau correspond au sien, il suit la direction.

---

## 17. Suivi d’un broadcast

Les directions de broadcast indiquent où aller.

Exemple :

```text
message 1, LEVELUP:2
```

Direction 1 signifie que la source est devant.

L’agent fait :

```text
Forward
```

Autres directions :

```text
2, 3, 4 -> tourner à gauche puis avancer
5       -> demi-tour puis avancer
6, 7, 8 -> tourner à droite puis avancer
0       -> déjà sur la même case
```

Si la direction vaut `0`, le joueur est arrivé sur la case du leader.

Dans ce cas, il ne lance pas forcément l’incantation lui-même. Il attend que le leader le fasse.

---

## 18. Leader et followers

Pour éviter que plusieurs joueurs lancent l’incantation en même temps, on distingue :

- le leader ;
- les followers.

### Leader

Le leader est le joueur qui appelle les autres avec :

```text
Broadcast LEVELUP:<level>
```

C’est lui qui doit lancer :

```text
Incantation
```

### Followers

Les followers sont les joueurs qui reçoivent le broadcast et se déplacent vers le leader.

Ils ne doivent pas lancer eux-mêmes l’incantation multi-joueurs.

Cette règle évite le bug où plusieurs bots envoient `Incantation` en même temps, ce qui peut créer des `ko` parasites dans le buffer.

Règle simple :

```text
niveau avec 1 joueur -> le joueur peut incanter seul
niveau avec plusieurs joueurs -> seul le leader incante
```

---

## 19. Appel des joueurs

Quand un joueur a les ressources mais pas assez de joueurs sur sa case, il appelle :

```text
Broadcast LEVELUP:<level>
```

Exemple niveau 2 :

```text
Players here: 1/2
Broadcast LEVELUP:2
```

Il attend ensuite que d’autres joueurs arrivent.

Quand il y a assez de joueurs :

```text
Players here: 2/2
```

le leader peut lancer l’incantation.

Si la case contient trop de joueurs :

```text
Players here: 3/2
```

l’incantation ne doit pas être lancée, car le nombre de joueurs doit être exact.

---

## 20. Fork

Le fork permet de créer un nouvel œuf pour l’équipe.

L’agent vérifie d’abord le nombre de slots disponibles :

```text
Connect_nbr
```

Si le serveur répond un nombre supérieur à 0 :

```text
Fork
```

Sinon, il attend.

Le fork est secondaire par rapport à la survie et aux incantations.

---

## 21. Rôles des bots

Chaque bot peut avoir un rôle choisi au hasard :

```python
WORKER
COLLECTOR
```

### WORKER

Le worker suit la logique générale :

- nourriture ;
- ressources ;
- incantation ;
- broadcast ;
- fork.

### COLLECTOR

Le collector est plus orienté collecte de ressources.

Il peut prioriser la recherche de ressources manquantes pour accélérer la progression globale.

Les rôles permettent d’éviter que tous les bots fassent exactement la même chose en permanence.

---

## 22. Exemple complet : passage niveau 1 vers niveau 2

### Situation initiale

Le joueur est niveau 1.

Il a besoin de :

```text
1 joueur
1 linemate
```

### Étape 1 : observe

Il lit son inventaire :

```text
food 10
linemate 0
```

Il regarde autour de lui :

```text
[ player, linemate, food ]
```

### Étape 2 : think

Il voit qu’il lui manque `linemate`.

Il choisit :

```text
SEARCH_LINEMATE
```

### Étape 3 : act

Il se déplace vers la linemate.

Puis il fait :

```text
Take linemate
```

### Étape 4 : prêt à incanter

Son inventaire contient :

```text
linemate 1
```

Il est seul sur la case.

Il choisit :

```text
LEVEL_UP
```

### Étape 5 : incantation

Il nettoie la case, pose la ressource :

```text
Set linemate
```

Puis :

```text
Broadcast LEVELUP:1
Incantation
```

Le serveur répond :

```text
Elevation underway
Current level: 2
```

L’agent met à jour :

```python
state.level = 2
```

---

## 23. Exemple complet : passage niveau 2 vers niveau 3

### Besoin du niveau 2

```text
2 joueurs
1 linemate
1 deraumere
1 sibur
```

### Étape 1 : collecte

Le bot collecte :

```text
linemate
deraumere
sibur
```

### Étape 2 : appel

Il voit qu’il est seul :

```text
Players here: 1/2
```

Il broadcast :

```text
Broadcast LEVELUP:2
```

Il devient leader.

### Étape 3 : follower

Un autre bot niveau 2 reçoit :

```text
message 4, LEVELUP:2
```

Il suit la direction 4.

Il continue jusqu’à recevoir :

```text
message 0, LEVELUP:2
```

ou jusqu’à être sur la même case que le leader.

### Étape 4 : incantation

Quand le leader voit :

```text
Players here: 2/2
```

il pose les ressources :

```text
Set linemate
Set deraumere
Set sibur
```

Puis :

```text
Incantation
```

Le serveur répond :

```text
Elevation underway
Current level: 3
```

Les deux joueurs passent niveau 3.

---

## 24. Problèmes rencontrés et corrections

### Problème 1 : messages asynchrones dans les mauvaises commandes

Avant, le bot pouvait recevoir :

```text
Elevation underway
```

comme réponse à `Look`.

Cela cassait tout le parsing.

Correction :

- le client détecte `Elevation underway` ;
- il lit jusqu’à `Current level: X` ou `ko` ;
- il stocke le résultat ;
- il continue à attendre la vraie réponse de la commande.

### Problème 2 : mauvais parsing d’inventaire

Quand les réponses étaient décalées, l’agent pouvait lire :

```text
ko
```

comme inventaire.

Résultat :

```python
Inventory: {}
```

Correction :

- centraliser la gestion du protocole dans le client ;
- ne plus faire de `read_line()` directement dans l’agent sauf si c’est strictement nécessaire.

### Problème 3 : trop de joueurs sur la case

Une incantation demande un nombre exact de joueurs.

Exemple :

```text
niveau 1 = 1 joueur
```

Si deux joueurs sont présents :

```text
Players here: 2/1
```

l’incantation ne doit pas être lancée.

Correction :

```python
players_here == required_players
```

et non :

```python
players_here >= required_players
```

### Problème 4 : trop de ressources sur la case

Si la case contient déjà une pierre et que l’agent pose encore la même pierre, l’incantation échoue.

Correction :

- nettoyer les pierres présentes ;
- poser exactement les pierres nécessaires.

### Problème 5 : plusieurs bots lancent l’incantation

Si plusieurs bots envoient `Incantation` en même temps, certains reçoivent `Current level`, d’autres `ko`, et les réponses peuvent se mélanger.

Correction :

- celui qui broadcast devient leader ;
- les autres deviennent followers ;
- seul le leader lance l’incantation multi-joueurs.

---

## 25. Règles importantes à respecter

### Règle 1

Le client gère le protocole réseau.

L’agent ne doit pas avoir à comprendre les détails du buffer TCP.

### Règle 2

L’agent décide quoi faire.

Il ne doit pas gérer les lignes asynchrones directement.

### Règle 3

Une incantation doit avoir exactement le bon nombre de joueurs.

Pas moins, pas plus.

### Règle 4

La case doit contenir exactement les bonnes pierres.

Pas de pierres en trop.

### Règle 5

Pour les niveaux multi-joueurs, seul le leader lance l’incantation.

### Règle 6

La nourriture est prioritaire sur les ressources.

Un bot mort ne peut plus monter de niveau.

---

## 26. Logs utiles pour debug

Pour comprendre ce que fait l’IA, les logs importants sont :

```text
Inventory: {...}
Vision: [...]
Messages: [...]
Following broadcast direction X
Players here: X/Y
Calling players for incantation
Cleaning tile: taking X
Incantation: Current level: X
LEVEL UPDATED: X
```

### Logs qui indiquent un bug

```text
Inventory: {}
```

Cela indique souvent que le bot a lu `ko` ou une autre mauvaise ligne comme inventaire.

```text
Vision: [['food', '7']]
```

Cela indique souvent que le bot a lu un inventaire comme si c’était un `Look`.

```text
Players here: 3/2
Incantation
```

Cela indique que le bot essaie d’incanter avec trop de joueurs.

```text
Incantation: ko
```

Cela peut venir de :

- mauvais nombre de joueurs ;
- mauvaise combinaison de ressources ;
- case pas nettoyée ;
- plusieurs bots qui incantent en même temps.

---

## 27. Résumé final

L’IA fonctionne avec une boucle simple :

```text
observe -> think -> act
```

Le client lit et nettoie le flux serveur.

L’agent prend les décisions.

Le state garde la mémoire.

Les parsers transforment les réponses texte en données exploitables.

Les bots survivent en cherchant de la nourriture, collectent les ressources, communiquent avec `Broadcast`, se regroupent, puis montent de niveau avec `Incantation`.

La partie la plus importante est la séparation des responsabilités :

```text
Client = protocole réseau
Agent = stratégie
State = mémoire
Parser = traduction texte -> données
Levels = règles d’incantation
```

Avec cette séparation, le code reste plus propre, plus facile à debug, et plus simple à améliorer.
