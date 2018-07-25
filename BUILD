cc_library(
    name = "menu",
    srcs = ["menu.cpp"],
    hdrs = ["menu.h"]
)

cc_library(
    name = "apple_picker_upgrade",
    hdrs = ["ApplePickerUpgrade.h"],
    deps = [
        "@com_google_absl//absl/strings:str_format",
    ],
)

cc_library(
    name = "magic_potion",
    hdrs = ["MagicPotion.h"],
    deps = [
        ":apple_picker_upgrade",
        "@com_google_absl//absl/strings:str_format",
    ],
)

cc_binary(
    name = "ApplesnGold",
    srcs = ["ApplesnGold.cpp"],
    deps = [
        ":menu",
        ":magic_potion",
        ":apple_picker_upgrade",
        "@com_google_absl//absl/strings:str_format",
    ],
)
