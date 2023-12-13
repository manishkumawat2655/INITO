#include <bits/stdc++.h>
using namespace std;

string time_now();// to get curr time



// Class that is representing a tree node in the file system
class TreeNode{
public:
    string name;// name of node

    list<string> content;// content of node or list of strings

    char type;// type of node  ('d' or directory and 'f' for file)
     /// node for date and modification date 
    string cdate;
    string mdate;
    // permissions
    int perm;

    TreeNode* link;
    
    //Pointer to the next node in the linked list (for file nodes)
    // Pointers to parent and child nodes in the tree structure
    TreeNode* parent;
    TreeNode* child;

      // constructor for new node creating
    TreeNode(TreeNode* pwd, string n)
    {
        name = n;// name 
        cdate = mdate = time_now();// set time 
        perm = 6;// rw- for simplicity
        // pointers set to NULL
        link = NULL;
        child = NULL;
        parent = pwd;
    }
    
    string get_perm()
    {
        string res;
        if (perm == 0) res = "---";
        else if (perm == 1) res = "--x";
        else if (perm == 2) res = "-w-";
        else if (perm == 3) res = "-wx";
        else if (perm == 4) res = "r--";
        else if (perm == 5) res = "r-x";
        else if (perm == 6) res = "rw-";
        else if (perm == 7) res = "rwx";

        return res;
    }
};
// functionality of differnt commands
void filesysytem_tree(TreeNode* root);
void print_help();
void print_ls(TreeNode* pwd);
string pwd_str(TreeNode* root, TreeNode* pwd);
list<string> find_names(TreeNode* root, TreeNode* pwd, string name);
TreeNode* find_node(TreeNode* root, TreeNode* pwd, string path);
TreeNode* find_on_pwd(TreeNode* pwd, string name);
list<string> split(string str, char delim);
string join(list<string> str, char delim);
string* split_name(string str);
TreeNode* cd(TreeNode* root, TreeNode* pwd, string path);
TreeNode* create(TreeNode* root, TreeNode* pwd, string path, char type);
void remove(TreeNode* root, TreeNode* pwd, string path);
void dupl(TreeNode* root, TreeNode* pwd, string src, string dst, int keep);
void cat(TreeNode* root, TreeNode* pwd, string path);

int main()
{
    TreeNode* root = new TreeNode(NULL, "");// root node create
    root->type = 'd';// as directory

    TreeNode* pwd = root;

    filesysytem_tree(root);// to see the file system tree structure

    print_help();// help information commands

    string cmd;

    cout << endl << pwd_str(root, pwd) << ">> ";

    // input loop for user commands
    while (getline(cin >> ws, cmd))
    {
        list<string> args = split(cmd, ' ');// split into arguments

        TreeNode* temp_pwd;// pointer temp for curr working direc.

        if (cmd == "help")
            print_help();
        else if (args.front() == "ls")// ls list all of them
        {
            if (args.size() == 1)
                print_ls(pwd->child);
            else while (args.size() != 1)
            {
                args.pop_front();// remove from argu. 
                cout << args.front() << ":" << endl;// display

                temp_pwd = cd(root, pwd, args.front());// change curr working directoryand print contents
                if (temp_pwd != NULL)
                    print_ls(temp_pwd->child);
            }
        }
        // if commnd is pwd curr . direc
        else if (cmd == "pwd"){
            cout << pwd_str(root, pwd) << endl;
        }
        // if cmd is cd 
        else if (args.front() == "cd")
        {
            if (args.size() == 1)
                pwd = root;
            else
            {
                args.pop_front();
                temp_pwd = cd(root, pwd, args.front());
                if (temp_pwd != NULL)
                    pwd = temp_pwd;
            }
            
        }
       // cmd is mkdir for create dire..
        else if (args.front() == "mkdir")
        {
            if (args.size() == 1)
                cout << "mkdir: missing operand" << endl;
            else while(args.size() != 1)
            {
                args.pop_front();
                create(root, pwd, args.front(), 'd');
            }
        }
        // cmd is touch for create file
        else if (args.front() == "touch")
        {
            if (args.size() == 1)
                cout << "touch: missing operand" << endl;
            else while(args.size() != 1)
            {
                args.pop_front();
                create(root, pwd, args.front(), '-');
            }
        }
        // rm cmd and rmdir for remove a file 
        else if (args.front() == "rm" || args.front() == "rmdir")
        {
            if (args.size() == 1)
                cout << args.front() << ": missing operand" << endl;
            else while(args.size() != 1)
            {
                args.pop_front();
                remove(root, pwd, args.front());
            }
        }
        // cp cmd for Copy a file to a destination or mv  Move a file to a new location
        else if (args.front() == "cp" || args.front() == "mv")
        {
            if (args.size() != 3)
                cout << args.front() << ": missing operand" << endl;
            else
            {
                int keep;
                if (args.front() == "cp") keep = 1;
                else keep = 0;
                args.pop_front();
                string src = args.front();
                args.pop_front();
                string dst = args.front();
                args.pop_front();
                dupl(root, pwd, src, dst, keep);
            }
        }
       // cat cmd for Display the contents of a file
        else if (args.front() == "cat")
        {
            if (args.size() == 1)
                cout << "cat: missing operand" << endl;
            else
            {
                args.pop_front();
                cat(root, pwd, args.front());
            }
        }

       // exit for exit the system
        else if (cmd == "exit")
            return 0;
        else
            cout << "Unknown command" << endl;
        
        cout << endl << pwd_str(root, pwd) << ">> ";
    }

    cout << endl;
    return 0;
}

void print_help()
{
    cout << "*** Follows the commands for file system  management ***" << endl << endl;
    cout << "\thelp - print following message" << endl;
    cout << "\tls - list directory" << endl;
    cout << "\tcd - change directory" << endl;
    cout << "\tpwd - print working directory" << endl;
    cout << "\tcp - copy file" << endl;
    cout << "\tmv - move file" << endl;
    cout << "\tmkdir - make directory" << endl;
    cout << "\trmdir - remove directory" << endl;
    cout << "\ttouch - create file" << endl;
    cout << "\trm - remove file" << endl;
    cout << "\tcat - Display file content" << endl;
    cout << "\texit - exit program" << endl;
}


// Func  to print the contents of a TreeNode in "ls" cmd
void print_ls(TreeNode* pwd)
{
    if (pwd != NULL)
    {
        cout << pwd->type << pwd->get_perm() << " " << pwd->mdate << " " << pwd->name;
        if (pwd->type == 'd') cout << "/";
        cout << endl;

        print_ls(pwd->link);
    }
}


//  func generate  string for the current working directory
string pwd_str(TreeNode* root, TreeNode* pwd)
{
    string res = "";
    if (pwd == root)
        return "/";
    while (pwd != root)
    {
        res = "/" + pwd->name + res;
        pwd = pwd->parent;
    }
    return res;
}
//  func to find TreeNode names containing a specific substring
list<string> find_names(TreeNode* root, TreeNode* pwd, string name)
{
    static list<string> res;
    if (root == pwd)
        res.clear();
    if (pwd != NULL)
    {
        string path = pwd_str(root, pwd);
        if (path.find(name.c_str()) != string::npos)
        {
            res.push_back(path);
        }
        find_names(root, pwd->child, name);
        find_names(root, pwd->link, name);
    }
    return res;
}
// Func to find a TreeNode based on a given path
// Takes the root of the tree, the current working directory (pwd), and the target path
TreeNode* find_node(TreeNode* root, TreeNode* pwd, string path)
{
    // Extract the parent path and name from the provided path
    string parent_path = split_name(path)[0];
    string name = split_name(path)[1];
    pwd = cd(root, pwd, parent_path);
    if (pwd == NULL)
    {
        return NULL;
    }
    return find_on_pwd(pwd, name);
}
// Func to find a TreeNode with a specific name in the children of the curr directory

TreeNode* find_on_pwd(TreeNode* pwd, string name)
{
    pwd = pwd->child;
    while (pwd != NULL)
    {
        if (pwd->name == name)
        {
            return pwd;
        }
        pwd = pwd->link;
    }
    return NULL;
}
// Function to split a string into a list of substrings based on a delimiter
list<string> split(string str, char delim)
{
    list<string> res;
    string temp = "";
    for (int i = 0; i < (int)str.size(); i++)
    {
        if (str[i] != delim)
            temp += str[i];
        else
        {
            res.push_back(temp);
            temp = "";
        }
    }
    if (temp != "")
        res.push_back(temp);
    return res;
}
// Function to join a list of strings into a single string with a specified delimiter
string join(list<string> str, char delim)
{
    string res = "";
    for (list<string>::iterator it = str.begin(); it != str.end(); ++it)
    {
        res += *it;
        if (it != --str.end())
            res += delim;
    }
    return res;
}
// Function to split a path into parent path and name
string* split_name(string str)
{
    string* res = new string[2];

    list<string> temp = split(str, '/');
    res[1] = temp.back();
    temp.pop_back();
    res[0] = join(temp, '/');

    return res;
}
// / Function to change the current working directory based on a specified path
TreeNode* cd(TreeNode* root, TreeNode* pwd, string path)
{
    list<string> path_list = split(path, '/');
    list<string>::iterator it = path_list.begin();
    TreeNode* temp;

    if (path[0] == '/')
    {
        temp = root;
        it++;
    }
    else if (path_list.front() == ".")
    {
        if (path_list.size() == 1)
            return pwd;
        else
        {
            temp = pwd;
            it++;
        }
    }
    else if (path_list.front() == "..")
    {
        if (path_list.size() == 1)
            return pwd->parent;
        else
        {
            temp = pwd->parent;
            it++;
        }
    }
    else
    {
        temp = pwd;
    }
    
    for (; it != path_list.end(); it++)
    {
        temp = find_on_pwd(temp, *it);
        if (temp == NULL || temp->type != 'd')
        {
            cout << "the directory '" << path << "' does not exist" << endl;
            return NULL;
        }
    }
    return temp;
}
// Function to create a new TreeNode with a specified path and type
TreeNode* create(TreeNode* root, TreeNode* pwd, string path, char type)
{
    string parent_path = split_name(path)[0];
    string name = split_name(path)[1];

    TreeNode* new_pwd = cd(root, pwd, parent_path);
    if (new_pwd == NULL)
    {
        cout << "the directory '" << parent_path << "' does not exist" << endl;
        return NULL;
    }

    TreeNode* temp = find_on_pwd(new_pwd, name);
    string choice = "";
    if (temp != NULL)
    {
        cout << "the file or directory '" << path << "' already exists" << endl;
        cout << "overwrite? (y/n) ";
        cin >> choice;
        if (choice != "y" && choice != "Y" && choice != "yes" && choice != "Yes")
            return NULL;
        remove(root, new_pwd, name);
    }

    temp = new TreeNode(new_pwd, name);
    temp->type = type;
    temp->parent = new_pwd;

    if (new_pwd->child == NULL)
    {
        new_pwd->child = temp;
        return temp;
    }
    TreeNode* prev = new_pwd->child;
    while (prev->link != NULL)
        prev = prev->link;
    prev->link = temp;

    return temp;
}
// Function to remove a TreeNode with a specified path
void remove(TreeNode* root, TreeNode* pwd, string path)
{
    TreeNode* temp = find_node(root, pwd, path);
    if (temp == NULL)
    {
        cout << "the file or directory '" << path << "' does not exist" << endl;
        return;
    }
    string choice = "";
    if (temp->type == 'd' && temp->child != NULL)
    {
        cout << "the directory '" << path << "' is not empty" << endl;
        cout << "proceed? (y/n) ";
        cin >> choice;
        if (choice != "y" && choice != "Y" && choice != "yes" && choice != "Yes")
            return;
    }

    if (temp->parent->child == temp)
    {
        temp->parent->child = temp->link;
        return;
    }
    TreeNode* prev = temp->parent->child;
    while (prev->link != temp)
        prev = prev->link;
    prev->link = temp->link;
}

// Function to duplicate a file or directory from source to destination
// Takes the root of the tree, the current working directory (pwd), source path,
// destination path, and a flag 'keep' indicating whether to keep the source after duplication
void dupl(TreeNode* root, TreeNode* pwd, string src, string dst, int keep)
{
    TreeNode* src_node = find_node(root, pwd, src);
    if (src_node == NULL)
    {
        cout << "the file or directory '" << src << "' does not exist" << endl;
        return;
    }
    string src_cdate = src_node->cdate;
    int src_perm = src_node->perm;
    list<string> src_content = src_node->content;

    TreeNode* dst_node = find_node(root, pwd, dst);
    if (dst_node != NULL && dst_node->type == 'd')
    {
        dst += "/" + src_node->name;
    }

    TreeNode* new_node = create(root, pwd, dst, src_node->type);
    if (new_node != NULL)
    {
        new_node->cdate = src_cdate;
        new_node->perm = src_perm;
        new_node->content = src_content;
    }
    else
        return;

    if (keep == 0)
    {
        remove(root, pwd, src);
    }

    return;
}
// Function to display the content of a file

void cat(TreeNode* root, TreeNode* pwd, string path)
{
    TreeNode* temp = find_node(root, pwd, path);
    if (temp == NULL)
    {
        cout << "the file '" << path << "' does not exist" << endl;
        return;
    }
    if (temp->type != '-')
    {
        cout << "'" << path << "' is not a file" << endl;
        return;
    }
    if (temp->perm < 4)
    {
        cout << "you don't have permission to read '" << path << "'" << endl;
        return;
    }
    for (list<string>::iterator it = temp->content.begin(); it != temp->content.end(); it++)
        cout << *it << endl;
}
// Function to return the current time in a specific format
// Returns a string representing the current time with day, month, hour, and minute

string time_now()
{
    string res = "", temp;
    time_t now = time(0);
    tm *ltm = localtime(&now);

    res += to_string(ltm->tm_mday) + " ";

    int month = ltm->tm_mon + 1;
    if (month == 1) res += "Jan ";
    else if (month == 2) res += "Feb ";
    else if (month == 3) res += "Mar ";
    else if (month == 4) res += "Apr ";
    else if (month == 5) res += "May ";
    else if (month == 6) res += "Jun ";
    else if (month == 7) res += "Jul ";
    else if (month == 8) res += "Aug ";
    else if (month == 9) res += "Sep ";
    else if (month == 10) res += "Oct ";
    else if (month == 11) res += "Nov ";
    else if (month == 12) res += "Dec ";

    if (to_string(ltm->tm_hour).size() == 1) res += "0";
    res += to_string(ltm->tm_hour) + ":";
    if (to_string(ltm->tm_min).size() == 1) res += "0";
    res += to_string(ltm->tm_min);

    return res;// Return the formatted time string
}
///  taking reference from the basic file stucture data to initiate structue
// Function to initialize a filesystem tree by creating directories and files
// Takes the root of the tree as a parameter
void filesysytem_tree(TreeNode* root)
{
    // list of directories 
    vector<string> Dirs{"bin", "etc", "etc/systemd", "etc/systemd/system", "etc/systemd/user", 
        "home", "home/user", "home/user/Desktop", "home/user/Documents", "home/user/Downloads", 
        "home/user/Pictures", "tmp"};
        // list of files
    vector<string> Files{"bin/bash", "bin/cd", "bin/gcc", "bin/ls", "bin/python", 
        "etc/systemd/system/display-manager.service", "etc/systemd/system.conf", "etc/systemd/user.conf", 
        "etc/hosts", "etc/passwd", "etc/profile", "home/user/.bashrc"};
// Create directories in the filesystem tree
    for (string d : Dirs)
    {
        create(root, root, d, 'd');
    }
    // Create files in the filesystem tree
    for (string f : Files)
    {
        create(root, root, f, '-');
    }
}