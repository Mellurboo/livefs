
![LiveFS Logo](http://livefs.chumnet.xyz/resources/hero.png)
# LiveFS : Simple C HTTP File Server
Livefs is a small C HTTP file server that enables users to download files via their browser, what makes livefs unique is its implementation of permissions.

For the future I aim to add more advanced features such as encryption and actual web pages to display links to the files at this point 
I will be able to add simple file pre-viewing.

Check out the official test server at `livefs.chumnet.xyz` and see it in action

## Permissions Philosophy
> [!IMPORTANT]  
> Below is implemented for the most part but still up to change, expect this part of the README to change

Permissions are done on a per-directory basis, The philosophy is that users have no permissions and can simply do readonly operations including downloading, It's down to the server host to manage permissions for everyone.

>[!NOTE]
> Thanks to the UK Govornment, I can not access my own imgur resources, as a result all images you see on this readme are hosted on a livefs server!

## Contributing
I am totally open to contributions, especally ones addressing security concerns. As usual feel free to fork the project just keep the licence in mind, this is free software for a reason!

*a breif shoutout in your readme would also be lovely <3*
## Appendix
I'm not doing anything ground breaking, if anything this is another learning experience for me which I have been doing a lot of recently, I hope to provide some value to someone who would like a simple lightweight HTTPS Download server that is simple to set up!

thank you for checking out my project.

Made with Love and C, by Mellurboo (and all the people in the Unmapped Nest Discord Server who helped me along the way)
