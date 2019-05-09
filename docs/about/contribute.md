
# Contributing to the Marlin project

## Found a bug? Want a new feature ?

The issues are collected via the Github issue tracker [https://github.com/iLCSoft/Marlin/issues](https://github.com/iLCSoft/Marlin/issues).
To open a new issue, clock on the button `New issue` and fill the issue template carefully to facilitate the developers's life (and so yours ...).

## Contributing to the code

To contribute, use the pull request functionalities provided by GitHub.

Fork the package with your GitHub account. Clone the original repository :

```shell
git clone https://github.com/iLCSoft/Marlin.git
cd Marlin
git remote add fork https://github.com/username/Marlin.git
git checkout -b dev
```

then work on the package as you need. When changes are done, do some commits :

git add 'modified files'
git commit -m "your commit message"
git push fork dev


Finally, go to the official GitHub package page [https://github.com/iLCSoft/Marlin/pulls](https://github.com/iLCSoft/Marlin/pulls) and create a new pull-request.

Please, follow the template form filling instructions in order to facilitate the review process by the authors. Note that the message will go in the release notes, so choose a meaningfull description.
