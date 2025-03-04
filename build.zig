const std = @import("std");

pub fn build(b: *std.Build) void {
    if (comptime !checkVersion())
        @compileError("Please! Update zig toolchain >= 0.11!");
    const target = b.standardTargetOptions(.{});
    const optimize = b.standardOptimizeOption(.{});

    const libvlcpp = b.addStaticLibrary(.{
        .name = "vlcpp",
        .optimize = optimize,
        .target = target,
    });
    libvlcpp.addCSourceFile(.{ .file = .{ .path = "test/empty.cpp" }, .flags = &.{} });
    b.installArtifact(libvlcpp);
    libvlcpp.installHeadersDirectory("vlcpp", "vlcpp");

    const examples = b.option([]const u8, "Example", "Build example: [helloworld, imem, renderers, test-vlcpp]") orelse return;
    if (std.mem.eql(u8, examples, "helloworld"))
        make_example(b, .{
            .mode = optimize,
            .target = target,
            .name = "helloworld",
            .path = "examples/helloworld/main.cpp",
        });

    if (std.mem.eql(u8, examples, "imem"))
        make_example(b, .{
            .mode = optimize,
            .target = target,
            .name = "imem",
            .path = "examples/imem/imem.cpp",
        });

    if (std.mem.eql(u8, examples, "renderers"))
        make_example(b, .{
            .mode = optimize,
            .target = target,
            .name = "renderers",
            .path = "examples/renderers/discovery.cpp",
        });

    if (std.mem.eql(u8, examples, "test-vlcpp")) {
        make_example(b, .{
            .mode = optimize,
            .target = target,
            .name = "test-vlcpp",
            .path = "test/main.cpp",
        });
    }
}

fn make_example(b: *std.Build, info: BuildInfo) void {
    const example = b.addExecutable(.{
        .name = info.name,
        .target = info.target,
        .optimize = info.mode,
    });
    if (info.mode != .Debug)
        example.strip = true;
    example.addIncludePath(.{ .path = "." });
    example.addCSourceFile(.{
        .file = .{ .path = info.path },
        .flags = &.{
            "-Wall",
            "-Wextra",
        },
    });

    if (info.target.isDarwin()) {
        // Custom path
        example.addIncludePath(.{ .path = "/usr/local/include" });
        example.addLibraryPath(.{ .path = "/usr/local/lib" });
        // Link Frameworks
        example.linkFramework("Foundation");
        example.linkFramework("Cocoa");
        example.linkFramework("IOKit");
        // example.linkFramework("Sparkle");
        // Link library
        example.linkSystemLibrary("vlc");
    } else if (info.target.isWindows()) {
        // msys2/clang - CI
        example.addIncludePath(.{ .path = msys2Inc(info.target) });
        example.addLibraryPath(.{ .path = msys2Lib(info.target) });
        example.linkSystemLibraryName("vlc.dll");
        example.want_lto = false;
    } else {
        example.linkSystemLibrary("vlc");
    }
    example.linkLibCpp();
    if (!std.mem.startsWith(u8, "test", info.name))
        b.installArtifact(example);

    const run_cmd = b.addRunArtifact(example);
    run_cmd.step.dependOn(b.getInstallStep());
    if (b.args) |args| {
        run_cmd.addArgs(args);
    }

    var descr = b.fmt("Run the {s}", .{info.name});
    const run_step = b.step("run", descr);
    run_step.dependOn(&run_cmd.step);
}

fn checkVersion() bool {
    const builtin = @import("builtin");
    if (!@hasDecl(builtin, "zig_version")) {
        return false;
    }

    const needed_version = std.SemanticVersion.parse("0.11.0") catch unreachable;
    const version = builtin.zig_version;
    const order = version.order(needed_version);
    return order != .lt;
}

const BuildInfo = struct {
    mode: std.builtin.OptimizeMode,
    target: std.zig.CrossTarget,
    name: []const u8,
    path: []const u8,
};

fn msys2Inc(target: std.zig.CrossTarget) []const u8 {
    return switch (target.getCpuArch()) {
        .x86_64 => "D:/msys64/clang64/include",
        .aarch64 => "D:/msys64/clangarm64/include",
        else => "D:/msys64/clang32/include",
    };
}

fn msys2Lib(target: std.zig.CrossTarget) []const u8 {
    return switch (target.getCpuArch()) {
        .x86_64 => "D:/msys64/clang64/lib",
        .aarch64 => "D:/msys64/clangarm64/lib",
        else => "D:/msys64/clang32/lib",
    };
}
