#include <ncurses.h>
#include <string>
#include <vector>
#include <sstream>
#include <dirent.h>

using namespace std;

struct Command {
    string name;
    vector<string> args;
};

struct Directory {
    string drive;
    string ldir;

    string win_dir()
    {
        vector<string> subDirs;
        istringstream iss(ldir);
        string token;

        while (getline(iss, token, '/')) {
            subDirs.push_back(token);
        }

        string winDir = drive + ":\\";
        for (unsigned int i = 1; i < subDirs.size(); i++)
        {
            if (subDirs[i] != "home" && subDirs[i] != "win") {
                winDir += subDirs[i] + "\\";
            }
        }
        if (winDir != (drive + ":\\")) {
            winDir.pop_back();
        }

        return winDir;
    }
};

Command ParseCommand(const string& input)
{
    Command cmd;
    istringstream iss(input);
    string token;

    if (iss >> token)
    {
        cmd.name = token;
    }

    while (iss >> token)
    {
        cmd.args.push_back(token);
    }

    return cmd;
}

string TranslateDir(string winDir) { // Optimize this
    vector<string> subDirs;
    istringstream iss(winDir);
    string token;

    while (getline(iss, token, '\\')) {
        subDirs.push_back(token);
    }

    string lxDir = "/home/win/";
    for (string subDir : subDirs) {
        lxDir += subDir + "/";
    }

    return lxDir;
}

void HandleCommand(const Command& cmd, Directory& dir) {
    if (cmd.name == "help") {
        printw("Available commands:\n");
        printw("  help - Show this help message\n");
        printw("  dir - List directory contents\n");
        printw("  cd - Change directory\n");
        printw("  exit - Exit the terminal\n");
        printw("  bash - Run any linux command within the DOS terminal\n");
    } else if (cmd.name == "bash") {
        if (cmd.args[0] == "exit")
        {
            endwin();
            std::exit(0);
        } else {
            string lcmd;
            for (string arg : cmd.args) {
                lcmd += arg + " ";
            }
            lcmd.pop_back();

            FILE* pipe = popen(lcmd.c_str(), "r");
            if (!pipe) {
                perror("FAILED TO EXECUTE COMMAND... RESUMING...");
            }

            char buffer[1024];
            while (fgets(buffer, sizeof(buffer), pipe) != NULL) {
                printw("%s", buffer);
            }

            pclose(pipe);
        }
    } else if (cmd.name == "cd" || cmd.name == "chdir") { // ** BROKEN **
        /* DIR* path = opendir();
        if (path == NULL) {
            printw("Invalid directory");
            return;
        }
        closedir(path);

        dir.ldir = TranslateDir(cmd.args[0]); */
    } else {
        printw("Bad command or file name\n");
    }
}

int main() {
    initscr();
    noecho();
    cbreak();

    Directory dir;
    dir.drive = "C";
    dir.ldir = "/home/win";

    string input;
    while (true) {
        flushinp();
        printw("%s>", dir.win_dir().c_str());

        int key = getch();
        int len = 0;
        while (key != '\n')
        {
            if (key == KEY_BACKSPACE || key == KEY_DC || key == 127) {
                if (len > 0) {
                    move(getcury(stdscr), getcurx(stdscr) - 1);
                    delch();
                    len--;
                    input.pop_back();
                }
            } else {
                input += key;
                printw("%c", key);
                len++;
            }
            refresh();
            key = getch();
        }
        printw("\n");

        Command cmd = ParseCommand(input);
        HandleCommand(cmd, dir);

        printw("\n");
        flushinp();
        input = "";
        refresh();
    }

    endwin();
    return -1;
}
