# Project 4 - Thornbound: Rise of the AI (archer & goblins update)

This version uses your new ruins PNG as the scrolling background, the GandalfHardcore archer as the animated player, and three animated goblins as the enemies. It keeps three levels, separate menu/win/lose Scene objects, shared lives, fixed time step updates, Map, Entity, 3 moving AI behaviors, and four sound effects.

## Controls
- Enter: start/return to menu
- A/D or left/right: move
- W or up: jump
- Space: shoot an arrow
- 1/2/3: jump to levels for debugging
- Q: quit

Three goblin enemy types: scout (patrol), tank (pursue), ranger (keep distance and shoot). Each enemy deals one life on contact, and ranger arrows deal one life.

## Windows PowerShell
Open this project folder so that `main.cpp`, `CS3113/`, and `assets/` are in the current directory.

```powershell
$env:Path += ";C:\raylib\w64devkit\bin"
g++ -std=c++11 main.cpp CS3113/cs3113.cpp CS3113/Entity.cpp CS3113/Map.cpp CS3113/Scene.cpp -o Rise_of_the_AI.exe -IC:\raylib\raylib\src -LC:\raylib\raylib\src -lraylib -lopengl32 -lgdi32 -lwinmm
.\Rise_of_the_AI.exe
```

## Asset origins & redistribution
- User-supplied GandalfHardcore Archer ZIP (repacked frames for in-game sprites). The included READ ME restricts reselling, repackaging or redistributing assets. Do not publish the raw art pack separately, and check the artist's license before posting game source with art in a public repository.
- User-supplied GoblinPack ZIP (repacked first-row animation frames); check its license before public publication.
- User-supplied ruins PNG is the background; confirm redistribution rights.
- Audio: `original_fantasy_loop.wav` is original. The alternate user-music version includes their uploaded track, which should not be publicly redistributed without permission.
- The original professor-inspired helpers are under `CS3113/`. `Entity` was minimally extended for the third AI and face direction. `Map` and `Scene` retain the preexisting project structure.

Review the assignment's academic integrity statement and class rules for assisted code before submission. Gameplay/physics should still be tested in your local Windows raylib environment.
