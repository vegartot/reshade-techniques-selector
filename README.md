# List required ReShade techniques

**This is a small executable program to help select the required techniques for your ReShade presets**

The program generates a text file you can use during ReShade installation to select all the required effects for your ReShade presets.

> ## *WHY ?*
> *I found it slightly annoying being only allowed to provide one ReShade preset during installation when certain games might benefit from multiple reshades. One solution is to always install all ReShade effects, but that affects the time required to swap between presets and is likely to make the experience slower.*



## How
1. Run the executable and a Open File Dialog will pop up.
2. Locate and select your desired ReShade presets via the open file dialog.
3. A text file <sub>*Reshade-Techniques.txt*</sub> is generated in the same working directory where your Reshade presets were selected from.
4. Install ReShade as normal, but select the generated text file on the effects page to have ReShade install the required effects.
5. Resume ReShade installation as normal.

> ### If you wish to add new presets requiring different effects, update the effects via the ReShade executable by repeating the steps above.

## BUILD
Use Microsoft Visual Studio build tools to build the program yourself with the provided [Makefile](Makefile)
```make
make main.obj
```
Alternatively download the [pre-compiled binaries](https://github.com/vegartot/reshade-techniques-selector/releases/tag/0.0.1)