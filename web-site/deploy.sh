cvs commit -m"Some changes, about to deploy."
ssh rj-cole@griff.sf.net "cd /home/groups/g/gr/griff/scripts/; cvs update; bash /home/groups/g/gr/griff/scripts/update_website.sh"
