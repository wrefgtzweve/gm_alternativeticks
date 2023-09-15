# gm_alternativeticks

Currently `sv_alternateticks` only affects the server when there's one player, not sure why this restriction is in place but i've seen a bunch of people wonder what it would do. This dll exposes a function to bypass this check. Currently the DLL only works on 32bit linux servers.

![image](https://github.com/wrefgtzweve/gm_alternativeticks/assets/69946827/1a3de047-6344-4696-b0fe-dd16621108e1)

## Lua

```lua
engine.EnableAlternativeTicks( bool ) -- Forces AlternativeTicks on/off
```
