mkdir .build\\Debug\\AppX\\Assets
mkdir .build\\Release\\AppX\\Assets


robocopy "Engine/Assets" ".build\\game_win64_debug\\AppX\\Assets" *.* /mir /w:0 /r:1 /v /log:".build\\game_win64_debug\\Assets.log"
robocopy "Engine/Assets" ".build\\game_win64_debug\\Assets" *.* /mir /w:0 /r:1 /v /log:".build\\game_win64_debug\\Assets.log"
robocopy "Engine/Assets" ".build\\game_win64_release\\AppX\\Assets" *.* /mir /w:0 /r:1 /v /log:".build\\Release\\Assets.log"
robocopy "Engine/Assets" ".build\\game_win64_release\\Assets" *.* /mir /w:0 /r:1 /v /log:".build\\Release\\Assets.log"

robocopy "Assets" ".build\\game_win64_debug\\AppX\\Assets" *.* /w:0 /r:1 /v /E /log:".build\\game_win64_debug\\Assets.log"
robocopy "Assets" ".build\\game_win64_debug\\Assets" *.* /w:0 /r:1 /v /E /log:".build\\game_win64_debug\\Assets.log"
robocopy "Assets" ".build\\game_win64_release\\AppX\\Assets" *.* /w:0 /r:1 /v /E /log:".build\\Release\\Assets.log"
robocopy "Assets" ".build\\game_win64_release\\Assets" *.* /w:0 /r:1 /v /E /log:".build\\Release\\Assets.log"

exit 0