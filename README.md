# cg_sdl

#Setting up repository...
git init
git status
git add .
git status
git commit -m "your commit message"
git status
git remote add origin "your http repository address"
git push origin master

#Submitting Changes...

git add -A
git status
git commit -m "your commit message"
git status
git push origin master

#Getting code...

git clone "your http repository address"

#Updating your code (to latest changes)

git pull origin master

NOTE: don't pull changes until you have added
	and commited any existing changes! (or else
	you will have a merge conflict)