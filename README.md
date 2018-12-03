# MacResForks

Assists in the removal of mac resourse fork files from non-mac filesystems.

MacResForks attempts to increase the accuracy of identifying actual mac resource fork files, by filtering the output of find for only those files that look like mac resource forks (ie: start with `._`) and have a corresponding regular file (a file of the same name without the `._` prefix in the same directory).  The assumption here being that resource fork files will generally have a corresponding regular file, whilst non-resource fork files that just happen to have a name that start with `._` generally wont.

A typical naieve approach one might use to purge mac resource fork files might be: 
```
find . -name '._*' -print0 | xargs -r -0 rm
```

Which can be improved by piping through `macresforks`, like so:
```
find . -name '._' -print0 | macresforks | xargs -0 -r rm
```

## Installation

```
mkdir ~/src
cd ~/src
git clone https://github.com/mlilley/macresforks.git
cd macresforks
make
sudo mv macresforks /usr/local/bin
```

Ensure `/usr/local/bin` is in your path somewhere, for example by adding this to your .bashrc or similar:
```
export PATH=$PATH:/usr/local/bin
```

## License

MIT