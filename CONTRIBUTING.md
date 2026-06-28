# Role distribution 

|	**Role**	|	**Responsibility**	|	**Assigned**	|
|----------|---------------------|-------------------|
|	**Repo Maintainer**	| 	Owns the GitHub repository configuration, manages branch protection, CI, secrets, and the Epitech mirror |	Léo Larcordaire, Noé Becquer |
|	**Server Owner**	| Owners of `zappy_server` development |	Jules Nourdin, Léo Lacordaire | 
|	**GUI Owner**	| 	Owner of `zappy_gui` development |	Younès Sarouti, Noé Becquer |
|	**AI Owner**	| 	Owners of `zappy_ai` development |	Mathilde Lorentz, Bassirou Tall |

---


# Communication among the team 

Communication between team members takes place on a Discord chat

> [Click here to join the chat](https://discord.gg/6gcZH3TT)

---

# Branch strategy

> All development happens on short-lived branches named after the feature or fix. When
complete, the branch is merged into main via a pull request. main is always in a deployable
state.

```zsh
main
 └── feature/server-player-movement
 └── feature/gui-map-renderer
 └── fix/ai-broadcast-parsing
 ```

Good for: fast iteration, simple mental model.

---

# Commit convention

> The commit convention is already very common among developpers. 
It is a simple yet effective convention that tells everything that is needed.


|	**Convention**	|	**Example**	|
|----------|---------------------|
| \<type>(\<scope>) : \<description> |	fix(server): prevent buffer overflow on oversized client commands |

- `<type>` : feat, fix, docs, chore, refactor, test, ci..
- `<scope>`: ai, server, gui, ci, docs...

# Review policy

> PR are manage by the repo manager. He should check each ones's to limit conflict between teams. He could allow another member to accept the request if he checked it before.

> It should be done on the same day, to keep a good pace in this project.
check if it works is mandatory but we shouldn't not take attention in code quality to keep the project easily maintanable.

# CI Pipeline

> It checks for each parts if all depencies are here, install them, build the project and for gui check if all tests run well.

/!\ Everything is mandatory !


