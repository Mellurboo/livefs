
![LiveFS Logo](https://i.imgur.com/imXs30i.png)
# LiveFS : Simple C HTTP File Server
Livefs is a small C HTTP file server that enables users to download files via their browser, what makes livefs unique is its implementation of permissions.

## Permissions Philosophy
> [!IMPORTANT]  
> Below is not currently implemented and is still a consept, everything is subject to change however the core consept should remain the same right up until it is finished!

Permissions are done on a per-directory basis, The philosophy is that users have no permissions and can simply do readonly operations including downloading, It's down to the server host to manage permissions for everyone. heres an example

```
# directory.cfg

# the first contentful line (comments excluded) should be the directory name
# within square brackets, this is to prevent you copying a folder and
# accidentally exposing it to the internet
[images]

# below is the settings for that directory, an example being "unlisted"
unlisted = false

# the ignorelist can contain files or entire directories.
[INGORELIST]
privateimage.png
homework/
```

the privilege level is higher at the top of a directory tree and gets lower
the lower down the tree you are.

![Privilege Diagram](https://i.imgur.com/M5JWzzJ.png)





## Contributing
I am totally open to contributions, especally ones addressing security concerns. As usual feel free to fork the project just keep the licence in mind, this is free software for a reason!

*a breif shoutout in your readme would also be lovely <3*
## Appendix
I'm not doing anything ground breaking, if anything this is another learning experience for me which I have been doing a lot of recently, I hope to provide some value to someone who would like a simple lightweight HTTPS Download server that is simple to set up!

thank you for checking out my project.

Made with Love and C, by Mellurboo.