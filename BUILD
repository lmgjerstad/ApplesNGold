cc_library(
    name = "menu",
    srcs = ["menu.cpp"],
    hdrs = ["menu.h"]
)

cc_library(
    name = "apple_picker_upgrade",
    hdrs = ["ApplePickerUpgrade.h"],
    srcs = ["ApplePickerUpgrade.cpp"],
    deps = [
        ":apples_n_gold_cc_proto",
        "@com_google_absl//absl/strings:str_format",
    ],
)

cc_library(
    name = "magic_potion",
    hdrs = ["MagicPotion.h"],
    srcs = ["MagicPotion.cpp"],
    deps = [
        ":apples_n_gold_cc_proto",
        "@com_google_absl//absl/strings:str_format",
    ],
)

cc_library(
    name = "idle_picker",
    hdrs = ["IdlePicker.h"],
    srcs = ["IdlePicker.cpp"],
    deps = [
        ":apples_n_gold_cc_proto",
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
        ":idle_picker",
        "@com_google_absl//absl/strings:str_format",
    ],
)

proto_library(
    name = "apples_n_gold_proto",
    srcs = ["apples_n_gold.proto"],
)

cc_proto_library(
    name = "apples_n_gold_cc_proto",
    deps = [":apples_n_gold_proto"],
)
