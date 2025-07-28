#include <iostream>
#include <string>
#include <fstream>
#include <cstdlib>
#include <ctime>
#include <unordered_map>
#include <functional>
using namespace std;

struct Song {
    string title;
    string artist;
    int duration;
    Song* next;
};

class Playlist {
private:
    Song* head;
    Song* current;

public:
    Playlist() {
        head = nullptr;
        current = nullptr;
    }

    void addSong(string title, string artist, int duration) {
        Song* newSong = new Song{title, artist, duration, nullptr};
        if (!head) {
            head = newSong;
            current = head;
        } else {
            Song* temp = head;
            while (temp->next)
                temp = temp->next;
            temp->next = newSong;
        }
        cout << "Added: " << title << " by " << artist << " (" << duration << "s)\n";
    }

    void promptAdd() {
        string title, artist;
        int duration;
        cout << "Enter title: ";
        getline(cin, title);
        cout << "Enter artist: ";
        getline(cin, artist);
        cout << "Enter duration (sec): ";
        cin >> duration;
        cin.ignore();
        addSong(title, artist, duration);
    }

    void displayPlaylist() {
        if (!head) {
            cout << "Playlist is empty.\n";
            return;
        }
        Song* temp = head;
        cout << "\nPlaylist:\n";
        while (temp) {
            cout << "- " << temp->title << " by " << temp->artist
                 << " [" << temp->duration << "s]";
            if (temp == current) cout << " [Current]";
            cout << '\n';
            temp = temp->next;
        }
    }

    void deleteSong(const string& title) {
        if (!head) {
            cout << "Playlist is empty.\n";
            return;
        }
        if (head->title == title) {
            Song* toDelete = head;
            head = head->next;
            if (current == toDelete)
                current = head;
            delete toDelete;
            cout << "Deleted song: " << title << '\n';
            return;
        }

        Song* temp = head;
        while (temp->next && temp->next->title != title)
            temp = temp->next;

        if (!temp->next) {
            cout << "Song not found.\n";
            return;
        }

        Song* toDelete = temp->next;
        temp->next = toDelete->next;
        if (current == toDelete)
            current = temp->next;
        delete toDelete;
        cout << "Deleted song: " << title << '\n';
    }

    void promptDelete() {
        string title;
        cout << "Enter title to delete: ";
        getline(cin, title);
        deleteSong(title);
    }

    void playCurrentSong() {
        if (!current) {
            cout << "No song selected.\n";
            return;
        }
        cout << "Now Playing: " << current->title << " by " << current->artist
             << " [" << current->duration << "s]\n";
    }

    void nextSong() {
        if (!current || !current->next) {
            cout << "End of playlist.\n";
            return;
        }
        current = current->next;
        playCurrentSong();
    }

    void previousSong() {
        if (!current || current == head) {
            cout << "Start of playlist.\n";
            return;
        }

        Song* temp = head;
        while (temp->next != current)
            temp = temp->next;
        current = temp;
        playCurrentSong();
    }

    void searchSong() {
        string title;
        cout << "Enter title to search: ";
        getline(cin, title);
        Song* temp = head;
        while (temp) {
            if (temp->title == title) {
                cout << "Found: " << temp->title << " by " << temp->artist
                     << " [" << temp->duration << "s]\n";
                return;
            }
            temp = temp->next;
        }
        cout << "Not found.\n";
    }

    void shuffleSong() {
        if (!head) {
            cout << "Playlist is empty.\n";
            return;
        }

        int count = 0;
        Song* temp = head;
        while (temp) {
            count++;
            temp = temp->next;
        }

        srand(time(0));
        int pos = rand() % count;

        temp = head;
        for (int i = 0; i < pos; i++)
            temp = temp->next;

        current = temp;
        cout << "Shuffle Play:\n";
        playCurrentSong();
    }

    void saveToFile(const string& filename) {
        ofstream fout(filename);
        Song* temp = head;
        while (temp) {
            fout << temp->title << '\n'
                 << temp->artist << '\n'
                 << temp->duration << '\n';
            temp = temp->next;
        }
        cout << "Playlist saved.\n";
    }

    void loadFromFile(const string& filename) {
        ifstream fin(filename);
        if (!fin) return;

        string title, artist;
        int duration;
        while (getline(fin, title) && getline(fin, artist) && fin >> duration) {
            fin.ignore();
            addSong(title, artist, duration);
        }
        cout << "Playlist loaded.\n";
    }

    ~Playlist() {
        Song* temp;
        while (head) {
            temp = head;
            head = head->next;
            delete temp;
        }
    }
};

int main() {
    Playlist myPlaylist;
    myPlaylist.loadFromFile("playlist.txt");

    unordered_map<string, function<void()>> commands;

    commands["add"]     = [&]() { myPlaylist.promptAdd(); };
    commands["show"]    = [&]() { myPlaylist.displayPlaylist(); };
    commands["delete"]  = [&]() { myPlaylist.promptDelete(); };
    commands["play"]    = [&]() { myPlaylist.playCurrentSong(); };
    commands["next"]    = [&]() { myPlaylist.nextSong(); };
    commands["prev"]    = [&]() { myPlaylist.previousSong(); };
    commands["search"]  = [&]() { myPlaylist.searchSong(); };
    commands["shuffle"] = [&]() { myPlaylist.shuffleSong(); };
    commands["help"]    = [&]() {
        cout << "Commands: add, show, delete, play, next, prev, search, shuffle, help, exit\n";
    };

    string input;

    cout << "Welcome to Terminal Music Playlist\n";
    cout << "Type 'help' to see available commands.\n";

    while (true) {
        cout << "\n> ";
        getline(cin, input);

        if (input == "exit") {
            myPlaylist.saveToFile("playlist.txt");
            cout << "Goodbye!\n";
            break;
        }

        auto cmd = commands.find(input);
        if (cmd != commands.end()) {
            cmd->second();
        } else {
            cout << "Unknown command. Type 'help' for options.\n";
        }
    }

    return 0;
}
