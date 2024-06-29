#include <FirebaseAuth.h>
#include <QTimer>

namespace DCS::UI
{
    FirebaseAuth::FirebaseAuth(const std::string_view& firebaseApiKey,
                               std::shared_ptr<DCS::Utils::Logger>& logger,
                               std::shared_ptr<RedisHandler>& redisHandler,
                               QObject* parent)
        : QObject(parent)
        , m_firebaseApiKey(firebaseApiKey)
        , m_logger(logger)
        , m_redisHandler(redisHandler)
    {
        connect(this, &FirebaseAuth::tokenRenewalConfirmed, this, &FirebaseAuth::promptTokenRenewal);
    }

    std::string FirebaseAuth::getFullUrl(const std::string& path) const
    {
        return "https://identitytoolkit.googleapis.com" + path + "?key=" + m_firebaseApiKey;
    }

    void FirebaseAuth::initializeTokenRenewal()
    {
        auto sessionTokenOpt = m_redisHandler->getToken("session_token");
        if (sessionTokenOpt.has_value())
        {
            auto expiresInOpt = m_redisHandler->getTTL("session_token");
            if (expiresInOpt.has_value() && expiresInOpt.value() > 0)
            {
                scheduleTokenRenewal(expiresInOpt.value());
            }
        }
    }

    void FirebaseAuth::scheduleTokenRenewal(int expiresIn)
    {
        QTimer::singleShot((expiresIn - 60) * 1000, this, &FirebaseAuth::promptTokenRenewal); // Prompt for token renewal 60 seconds before it expires
    }

    void FirebaseAuth::promptTokenRenewal()
    {
        emit tokenRenewalRequested();
    }

} // namespace DCS::UI
