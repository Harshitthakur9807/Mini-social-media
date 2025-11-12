#include <iostream>
#include <string>
#include <vector>
#include <map>
#include <algorithm>
#include <limits>

using namespace std;

// ============================= CLASS DEFINITIONS =============================

class Notification {
    string message;
    bool read;
public:
    Notification(string msg) : message(msg), read(false) {}
    void display() const {
        cout << (read ? "[Seen] " : "[New] ") << message << endl;
    }
    void markAsRead() { read = true; }
    bool isRead() const { return read; }
};

class Post {
public:
    static int nextId;
    int postId;
    string content;
    vector<string> comments;
    int likes;

    Post(string txt) {
        postId = nextId++;
        content = txt;
        likes = 0;
    }

    void addLike() { likes++; }
    void addComment(string c) { comments.push_back(c); }

    void display() const {
        cout << "Post ID: " << postId << " | " << content << " | Likes: " << likes << endl;
        if (!comments.empty()) {
            cout << "Comments:" << endl;
            for (auto &c : comments) cout << " - " << c << endl;
        }
    }
};
int Post::nextId = 1;

// ----------------------------- USER CLASS --------------------------------

class User {
public:
    string fullName, username, password, dob, college, address;
    vector<User*> friends;
    vector<Post> posts;
    vector<Notification> notifications;
    vector<string> friendRequests;
    map<string, vector<string>> inbox; // messages received

    User() {}
    User(string fn, string un, string pw, string db, string clg, string addr)
        : fullName(fn), username(un), password(pw), dob(db), college(clg), address(addr) {}

    void viewProfile() {
        cout << "\n----- PROFILE -----\n";
        cout << "Name: " << fullName << endl;
        cout << "Username: " << username << endl;
        cout << "DOB: " << dob << endl;
        cout << "College: " << college << endl;
        cout << "Address: " << address << endl;
        cout << "Friends: " << friends.size() << endl;
    }

    void addNotification(string msg) {
        notifications.push_back(Notification(msg));
    }

    void showNotifications() {
        cout << "\n🔔 Notifications for " << username << ":\n";
        if (notifications.empty()) cout << "No new notifications.\n";
        for (auto &n : notifications) n.display();
        for (auto &n : notifications) n.markAsRead();
    }

    void addPost(string txt) {
        posts.push_back(Post(txt));
        cout << "✅ Post created successfully! (Post ID: " << posts.back().postId << ")\n";
        for (auto &f : friends)
            f->addNotification("Your friend " + username + " added a new post!");
    }

    void viewMyPosts() {
        cout << "\n----- Your Posts -----\n";
        if (posts.empty()) cout << "No posts yet.\n";
        for (auto &p : posts) p.display();
    }

    void viewAllPosts(vector<User*> &users) {
        cout << "\n----- All Posts -----\n";
        for (auto u : users) {
            for (auto &p : u->posts) {
                cout << "[" << u->username << "] ";
                p.display();
            }
        }
    }

    void deletePost(int pid) {
        for (auto it = posts.begin(); it != posts.end(); ++it) {
            if (it->postId == pid) {
                posts.erase(it);
                cout << "🗑️ Post deleted successfully!\n";
                for (auto &f : friends)
                    f->addNotification(username + " deleted a post you interacted with.");
                return;
            }
        }
        cout << "❌ Post not found!\n";
    }

    void likePost(vector<User*> &users, int pid) {
        for (auto u : users) {
            for (auto &p : u->posts) {
                if (p.postId == pid) {
                    p.addLike();
                    u->addNotification(username + " liked your post (ID: " + to_string(pid) + ")");
                    cout << "👍 You liked post " << pid << endl;
                    return;
                }
            }
        }
        cout << "❌ Post not found!\n";
    }

    void commentOnPost(vector<User*> &users, int pid, string txt) {
        for (auto u : users) {
            for (auto &p : u->posts) {
                if (p.postId == pid) {
                    p.addComment(txt);
                    u->addNotification(username + " commented on your post.");
                    cout << "💬 Comment added to post " << pid << endl;
                    return;
                }
            }
        }
        cout << "❌ Post not found!\n";
    }

    void sendFriendRequest(vector<User*> &users, string uname) {
        for (auto u : users) {
            if (u->username == uname) {
                u->friendRequests.push_back(username);
                u->addNotification("New friend request from " + username);
                cout << "✅ Friend request sent to " << uname << endl;
                return;
            }
        }
        cout << "❌ User not found!\n";
    }

    void showFriendRequests() {
        if (friendRequests.empty()) {
            cout << "No friend requests.\n";
            return;
        }
        cout << "\n----- Friend Requests -----\n";
        for (size_t i = 0; i < friendRequests.size(); i++)
            cout << i + 1 << ". " << friendRequests[i] << endl;
    }

    void acceptFriendRequest(vector<User*> &users, string uname) {
        auto it = find(friendRequests.begin(), friendRequests.end(), uname);
        if (it == friendRequests.end()) {
            cout << "❌ No request from " << uname << endl;
            return;
        }
        friendRequests.erase(it);
        User* requester = nullptr;
        for (auto u : users)
            if (u->username == uname) requester = u;
        if (requester) {
            friends.push_back(requester);
            requester->friends.push_back(this);
            requester->addNotification(username + " accepted your friend request!");
            cout << "✅ You are now friends with " << uname << "!\n";
        }
    }

    void showFriends() {
        cout << "\n----- Friends -----\n";
        if (friends.empty()) cout << "No friends added yet.\n";
        for (auto f : friends)
            cout << f->username << endl;
    }

    void sendMessage(string uname, string msg, vector<User*> &users) {
        for (auto u : users) {
            if (u->username == uname) {
                u->inbox[username].push_back(msg);
                u->addNotification("New message from " + username);
                cout << "📩 Message sent to " << uname << endl;
                return;
            }
        }
        cout << "❌ User not found!\n";
    }

    void viewMessages() {
        cout << "\n----- Inbox -----\n";
        if (inbox.empty()) cout << "No messages yet.\n";
        for (auto &pair : inbox) {
            cout << "From " << pair.first << ":\n";
            for (auto &m : pair.second)
                cout << " - " << m << endl;
        }
    }
};

// ============================= HELPER FUNCTIONS =============================

User* findUser(vector<User*> &users, string uname) {
    for (auto u : users)
        if (u->username == uname)
            return u;
    return nullptr;
}

// ============================= MAIN PROGRAM =============================

int main() {
    vector<User*> users;
    User* current = nullptr;
    int choice;

    while (true) {
        cout << "\n===== MINI SOCIAL MEDIA APP =====\n";
        cout << "1. Login\n2. Sign Up\n3. Exit\nChoice: ";
        cin >> choice;
        cin.ignore();

        if (choice == 1) {
            string uname, pass;
            cout << "Enter Username: "; getline(cin, uname);
            cout << "Enter Password: "; getline(cin, pass);

            User* u = findUser(users, uname);
            if (u && u->password == pass) {
                current = u;
                cout << "✅ Login successful! Welcome, " << current->fullName << "!\n";

                // ----------- USER DASHBOARD -----------
                int opt;
                while (true) {
                    cout << "\n----- MAIN INTERFACE -----\n";
                    cout << "1. View Profile\n2. View Notifications\n3. Add Friend\n4. View Friend Requests\n5. Accept Friend Request\n6. Show Friends\n7. Add Post\n8. View My Posts\n9. View All Posts\n10. Like Post\n11. Comment on Post\n12. Delete Post\n13. Send Message\n14. View Messages\n15. Logout\n";
                    cout << "Choice: ";
                    cin >> opt;
                    cin.ignore();

                    if (opt == 1) current->viewProfile();
                    else if (opt == 2) current->showNotifications();
                    else if (opt == 3) {
                        string uname; cout << "Enter username to send friend request: "; getline(cin, uname);
                        current->sendFriendRequest(users, uname);
                    }
                    else if (opt == 4) current->showFriendRequests();
                    else if (opt == 5) {
                        string uname; cout << "Enter username to accept: "; getline(cin, uname);
                        current->acceptFriendRequest(users, uname);
                    }
                    else if (opt == 6) current->showFriends();
                    else if (opt == 7) {
                        string txt; cout << "Enter post content: "; getline(cin, txt);
                        current->addPost(txt);
                    }
                    else if (opt == 8) current->viewMyPosts();
                    else if (opt == 9) current->viewAllPosts(users);
                    else if (opt == 10) {
                        int pid; cout << "Enter Post ID to like: "; cin >> pid; cin.ignore();
                        current->likePost(users, pid);
                    }
                    else if (opt == 11) {
                        int pid; cout << "Enter Post ID to comment on: "; cin >> pid; cin.ignore();
                        string c; cout << "Enter your comment: "; getline(cin, c);
                        current->commentOnPost(users, pid, c);
                    }
                    else if (opt == 12) {
                        int pid; cout << "Enter Post ID to delete: "; cin >> pid; cin.ignore();
                        current->deletePost(pid);
                    }
                    else if (opt == 13) {
                        string uname, msg;
                        cout << "Enter friend's username: "; getline(cin, uname);
                        cout << "Enter message: "; getline(cin, msg);
                        current->sendMessage(uname, msg, users);
                    }
                    else if (opt == 14) current->viewMessages();
                    else if (opt == 15) {
                        cout << "👋 Logged out successfully!\n";
                        break;
                    }
                    else cout << "❌ Invalid option!\n";
                }
            } else cout << "❌ Invalid credentials!\n";
        }

        else if (choice == 2) {
            string fn, un, pw, db, clg, addr;
            cout << "Enter Full Name: "; getline(cin, fn);
            cout << "Enter Username: "; getline(cin, un);
            cout << "Enter Password: "; getline(cin, pw);
            cout << "Enter Date of Birth: "; getline(cin, db);
            cout << "Enter College: "; getline(cin, clg);
            cout << "Enter Address: "; getline(cin, addr);

            if (findUser(users, un)) {
                cout << "❌ Username already taken!\n";
            } else {
                users.push_back(new User(fn, un, pw, db, clg, addr));
                cout << "✅ Account created successfully!\n";
            }
        }

        else if (choice == 3) {
            cout << "👋 Exiting program. Goodbye!\n";
            break;
        }

        else cout << "❌ Invalid choice!\n";
    }

    return 0;
}
