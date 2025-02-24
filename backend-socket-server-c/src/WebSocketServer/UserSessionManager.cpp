#include "UserSessionManager.h"
#include <nlohmann/json.hpp>
#include <spdlog/spdlog.h>

// Get the singleton instance
UserSessionManager &UserSessionManager::getInstance()
{
    static UserSessionManager instance;
    return instance;
}

// Add session: Store in memory & Redis
void UserSessionManager::addSession(const std::string &userId, std::shared_ptr<WebSocketSession> session)
{
    std::unique_lock lock(sessionMutex_);
    userSessions_[userId] = session;
    spdlog::info("Session added for user {}", userId);
}
// Get session: Check memory first, then Redis
std::shared_ptr<WebSocketSession> UserSessionManager::getSession(const std::string &userId)
{
    std::shared_lock lock(sessionMutex_);
    auto it = userSessions_.find(userId);
    if (it != userSessions_.end())
    {
        return it->second;
    }

    spdlog::warn("Session not found for user {}", userId);
    return nullptr; // Reject the request if session is not found
}

// Remove session: Delete from memory & Redis
void UserSessionManager::removeSession(const std::string &userId)
{
    std::unique_lock lock(sessionMutex_);
    userSessions_.erase(userId);

    spdlog::info("Session removed for user {}", userId);
}

// Check if session exists
bool UserSessionManager::hasSession(const std::string &userId)
{
    std::shared_lock lock(sessionMutex_);
    return userSessions_.find(userId) != userSessions_.end();
}
