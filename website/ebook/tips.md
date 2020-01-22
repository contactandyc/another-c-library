---
path: "/tips"
posttype: "docs"
title: "Helpful Tips"
---

A list of tips that have been helpful to me:

## Line spacing in markdown
Just use a br html tag to create a single line break.  

## Escape characters in markdown
Use the \\ to escape characters in markdown such as the underscore or asterisk.

## Create multiline macro in C
On a mac, Command-F to get find and replace to show up on the bottom.  Next, select the [<b>\.\*</b>] button on the right side of the find and replace pane.  Replace dollar (<b>$</b>) with a space followed by a backslash (<b> \\</b>).  If you have atom-beautify installed, you can beautify (I have beautify on save enabled for the C and C++ languages and clang-format as beautifier).  If you have trouble with Atom-beautify, see next tip.

## Static inline vs inline
At least on my mac, when I compile code into binaries in debug mode (-g vs -O3), I get a linking error for functions which were declared inline vs static inline.

## Regex find replace
```
Find: less\(([^,]*),([^),]*)\)
Replace: less($1,$2, arg)
```

To find and replace all instances of less(a, b) with less(a, b, arg)

## Atom-beautify problems
I use Atom with atom-beautify when coding.  Recently, I ran across an error where I couldn't get it to work with C/C++ code.  I had to do the following to get it to work.

Install llvm
```
brew install llvm
```

Add path in Atom (path may be different to llvm)<br/>
Atom => Preferences => Packages => atom-beautify => Settings => Executables => ClangFormat<br/>
```
/usr/local/Cellar/llvm/9.0.0/bin
```

Fix coffee script per https://github.com/Glavin001/atom-beautify/issues/2290

In the file,
```
~/.atom/packages/atom-beautify/src/beautifiers/clang-format.coffee
```

At line 84
```coffee
return @exe("clang-format").run([
  @dumpToFile(dumpFile, text)
  ["--style=file"]
  ]).finally( ->
    fs.unlink(dumpFile)
  )
```

Add the -> in the fs.unlink to look like
```coffee
return @exe("clang-format").run([
  @dumpToFile(dumpFile, text)
  ["--style=file"]
  ]).finally( ->
    fs.unlink(dumpFile, ->)
  )
```
