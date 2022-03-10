# (Mini)-Scheme Virtual Machine (version étudiant)

fork from https://gitlab.com/lu3in018-public/svm

Ce projet contient les sources de la machine virtuelle SVM
utilisée dans le cadre du cours de compilation en L3 à
Sorbonne Université.

## Compilation

Pour compiler la machine virtuelle, il faut que le compilateur `scompiler` soit installé dans le même répertoire que celui de la machine virtuelle `svm`. Et il faut de plus :

 - le compilateur `gcc`
 - l'outil `make`

(pour des conseils d'installation de l'environnement sous windows, consultez le fichier `INSTALLATION-win.txt`)

Pour la compilation on ouvrira un terminal  (*bash* sous Linux/Unix/MacOSX, *powershell* sous windows) ira dans le sous-répertoire `src/` et on tapera :

```
make
```

Si tout se passe bien, un exécutable `svm`  (ou `svm.exe` sous windows) sera généré.

Pour les différentes commandes disponibles : `svm --help`  (ou `svm.exe --help` sous windows)

----
Copyright (C) 2021- F.P. under the LGPLv3 (cf. LICENSE)


