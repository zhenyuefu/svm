
# Installation des outils de développement GNU sous windows

La SVM est développée en langage C, avec gcc (*GNU Compilers Collection*) avec la bibliothèque standard C, et elle utilise également l'outil `make` pour la construction.

Ces outils ne sont pas disponibles par défaut sous Windows, contrairement à Linux notamment.

Il existe plusieurs manières d'obtenir ces outils sous windows, notamment :

 1. Utiliser un gestionnaire de paquetages comme *Chocolatey*

 2. Installer un VM (par exemple Virtual Box ou VMWare) avec Linux (par exemple Ubuntu ou autre)

 3. Installer l'environnement MSYS2

 4. Installer l'environnement WSL2  (Windows Subsystem for Linux, v2)

 5. Installer l'environnement Cygwin


Si votre machine est assez puissante, nous vous conseillons fortement d'installer une VM avec Linux, ce sera un environnement de travail idéal pour les UE de programmation, entre autre.

Sinon, la méthode à notre avis la plus simple est d'installer **Chocolatey**, et c'est la méthode qui sera décrite dans ce document.

## Installation de Chocolatey

*Chocolatey* est un gestionnaire de Paquets logiciels, qui s'utilise depuis le terminal *Powershell*.   Pour installer *Chocolatey*, il faut aller sur la page suivante : <https://chocolatey.org/install>  et suivre les instructions.

**IMPORTANT** : nous donnons les indications ci-dessous à titre informel, et sans **aucune garantie** quand à la sécurité et au bon fonctionnement de la démarche. L'outil *Chocolatey* est utilisé par des millions de personnes, et est donc relativement sûr, mais il nécessite de passer en *mode administrateur*.

**REMARQUE** : nous recommandons l'installation de la version gratuite (non licenciée) et en mode individuel (*individual*)... Ce sont normalement les options par défaut. 


La première chose à faire est de lancer le terminal *Powershell* en *mode administrateur*.  Sous Windows 10, il suffit de cliquer avec le bouton droit de la souris sur l'icône "fenêtre" en bas à gauche, et de choisir *Windows Powershell (admin)*.

Une fois le terminal lancé, suivez les instructions qui vous invitent à copier/coller une commande très longue, pour l'exécuter dans le terminal.  Cette commande installe le script principal de *Chocolate*, qui s'appelle `install.ps1`.

Une fois cette commande exécutée, vous pouvez vérifier l'installation en tappant dans le terminal *powershell* :

```
choco /?
```

Cela devrait indiquer les différentes options de la commande `choco` avec laquelle on installe des paquets logiciels.

## Installation de gcc et make

Pour installer gcc et make, il suffit, toujours dans le terminal *Powershell* en *mode administrateur*, de taper la commande suivante :

```
choco install mingw

```

(vous pouvez accepter les demandes avec `Y` pour `Yes`).

Normalement, après installation de l'environnement `mingw` le compilateur `gcc` et l'outil `make` devraient être disponibles. Pour tester, vous pouvez saisir les commandes suivantes :

```
gcc --help
```

et

```
make --help
```

Vous pouvez maintenant fermer votre


## Compilation de la SVM

Pour compiler la SVM, il faut maintenant ouvrir un *Powershell* en mode utilisateur  (*pas* administrateur). Il faut ensuite se déplacer dans le répertoire `src/` de la SVM et simplement taper :

```
make
```

Normalement, la compilation devrait fonctionner.


## Erreurs ? Suggestions ?

N'hésitez pas à proposer des modifications à ce document, soit par des *Merge Requests* (MR) sur le projet, soit en nous contactant directement.

----

**Bonne compilation !**

(et tout serait plus simple sous Linux ...)

