workspace(name = "ImguiSFMLBazel")

load("@bazel_tools//tools/build_defs/repo:http.bzl", "http_archive")

# Imgui repository located inside your ThirdParty library folder
local_repository(
  name = "imguilib",
  path = "ThirdParty/imgui/",
)

# Catch2 testing framework
http_archive(
    name = "catch2",
    urls = ["https://github.com/catchorg/Catch2/archive/v3.4.0.tar.gz"],
    strip_prefix = "Catch2-3.4.0",
    sha256 = "122928b814b75717316c71af69bd2b43387643ba076a6ec16e7882bfb2dfacbb"
)
