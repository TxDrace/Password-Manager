<h3 align="center">Password Manager Tool</h3>

---

This is a simple command line tool for securely storing passwords. They are encrypted using the hash value of your master password and stored on mongoDB database. I created this tool for personal use, ultilize a free M0 cluster on MongoDB Atlas. To use it, you'll need to create your own database on MongoDB Atlas.

Remember your master password! If you forget it, you will lose everything. However, your master password will be stored in your system's environment variables. Don't change it unless you reset the master password.

Enjoy!

## Prerequisites
Before downloading, make sure you have a database set up on MongoDB Atlas. If you don't, follow these steps:

- **Create an Account**: If you're new to MongoDB Atlas, sign up for a free account on their website.

- **New Project**: Once you're logged in, create a new project within MongoDB Atlas.

- **Free Cluster**: Within your project, create a new cluster. The M0 tier cluster is free and suitable for this application.

- **Connection String**: Locate the connection string for your cluster. You'll find it in the MongoDB Atlas dashboard. Store this string as a system environment variable named `MONGODB_URI`.

**Important**:
- Ensure your database can be accessed from anywhere by adding the IP address `0.0.0.0/0` to the IP Whitelist in the Network Access tab of your cluster settings.
- Create a new database named `password_manager` within your cluster. This is where the application will store your passwords.

## Installation

Now, download the latest release zip file from the release page. Extract the contents to a directory of your choice. This folder contains the `pm.exe` binary and necessary DLL files.

Don't forget to add the directory path where you extracted the files to your system's PATH environment variable. This allows you to run the program from any location. You may need to restart your computer for the changes to take effect.

Finally, set the following environment variables. These are essential for the program to connect to your database:

- **`MONGODB_URI`**: Paste the connection string for your MongoDB Atlas cluster. You can find this in your MongoDB Atlas dashboard.
- **`PM_MASTER_PASSWORD`**: Set a strong master password. This will be used to encrypt and protect your passwords.

Done! You can now use the command `pm` in your command prompt or powershell.



