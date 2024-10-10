# Installing CmdSeqEdit

This document covers the different ways you can install CmdSeqEdit on its supported
platforms:

- Windows 7, 8, 8.1, 10, 11 (64-bit)
- Linux (64-bit)

## Windows

1. You will need to install the VS2015-2022 runtime located at https://aka.ms/vs/17/release/vc_redist.x64.exe.
2. When that is installed, download either the standalone version or the installer version from [the latest
   GitHub release](https://github.com/craftablescience/CmdSeqEdit/releases/latest), under the `Assets` dropdown.
   - Note that if you are using Windows 7, 8, or 8.1, you will need to download the "Compatibility" version of the GUI for
     it to work. The CLI will work on any OS.
3. If you downloaded the standalone files, you're done, just unzip the files. If you downloaded the installer,
   unzip the installer application and run it. When running the application Windows will give you a safety warning,
   ignore it and hit `More Info` → `Run Anyway`.

## Linux

Installation on Linux will vary depending on your distro. On all distros you should be able to run the standalone
application without any issues.

> [!TIP]
> The installer will symlink the application to `/usr/bin` automatically,
> but if you prefer or need to use a standalone version, you can run one or both of the following commands to add them to the PATH:
>
> ```sh
> ln -s /path/to/standalone/cmdseqedit /usr/bin/cmdseqedit
> ```
>
> With the symlink in place, updating your standalone install will automatically update the binary on the PATH.

There are two ways of *installing* CmdSeqEdit specific to the following distros:

#### Debian-based:

###### Automatic:

1. Visit https://craftablescience.info/ppa/ and follow the instructions.
2. CmdSeqEdit should now be installable and upgradable from `apt` (the package name being `cmdseqedit`).

###### Manual:

1. Download the installer from the GitHub releases section, and extract the `.deb` file from inside.
2. Run `sudo apt install ./<name of deb file>.deb` in the directory you extracted it to.
