#ifndef USER_SESSION_MANAGER_H
#define USER_SESSION_MANAGER_H

#include <unordered_map>
#include <memory>
#include <string>
#include <shared_mutex>
#include "WebSocketSession.h"

class UserSessionManager
{
public:
    static UserSessionManager &getInstance();

    void addSession(const std::string &userId, std::shared_ptr<WebSocketSession> session);
    std::shared_ptr<WebSocketSession> getSession(const std::string &userId);
    void removeSession(const std::string &userId);
    bool hasSession(const std::string &userId);

private:
    UserSessionManager() = default;
    UserSessionManager(const UserSessionManager &) = delete;
    UserSessionManager &operator=(const UserSessionManager &) = delete;

    std::unordered_map<std::string, std::shared_ptr<WebSocketSession>> userSessions_;
    std::shared_mutex sessionMutex_;
};

#endif // USER_SESSION_MANAGER_H
