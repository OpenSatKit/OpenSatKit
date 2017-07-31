Adding a feature/fix to the master branch

```bash
# get the current code base
git clone https://github.com/OpenSatKit/OpenSatKit.git

cd OpenSatKit

# check what branch you are on (should be master)
git branch

# switch to development branch
git checkout development

# make your changes

# commit your changes to development
git add <file>
git commit -m "<your change>"
git push

# switch to master branch
git checkout master

# merge changes from development into master
git merge development

# push the merge
git push
```
