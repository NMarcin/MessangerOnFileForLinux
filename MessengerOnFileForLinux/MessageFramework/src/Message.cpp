#include <Message.hpp>
#include <string>
#include <StringSum.hpp>

Message::Message(MessageFlag messageFlag, std::string username, std::string content)
{
    setMessageFlag(messageFlag);        // TODO mnurzyns use throw as false check
    setUsername(username);
    setContent(content);
}

std::string Message::messageToSave()
{
    StringSumSquareBrackets fullMessage;

    fullMessage.sum(std::to_string(static_cast<int>(messageFlag_)));
    fullMessage.sum(date_ + " | " + time_);
    fullMessage.sum(username_);
    fullMessage.sum(content_);

    return fullMessage.getSumedString();
}


bool Message::setMessageFlag(MessageFlag messageFlag)
{
    if(MessageFlag::inviterMessage == messageFlag || MessageFlag::recipientMessage == messageFlag)
    {
        messageFlag_ = messageFlag;
        return true;
    }
    else if (MessageFlag::readMessage == messageFlag)
    {
        return false; // wiadomosc byla juz odczytana
    }
    else
    {
        return false;
    }
}

bool Message::setUsername(std::string username)
{
    int requiredUsernameSize = 8; // for example 'mnurzyns'
    if(username.size() == requiredUsernameSize)
    {
        username_ = username;
        return true;
    }
    else
    {
        return false;
    }
}

bool Message::setContent(std::string content)
{
    int firstLetter = 1;
    int secondLetter = 2;
    if(content[firstLetter] != '/' && content[secondLetter] != '/')
    {
        content_ = content;
        return true;
    }
    else
    {
        // it's not message, it's terminal command!
        return false;
    }
}

std::string Message::getTime()
{
    return time_;
}

std::string Message::getUsername()
{
    return username_;
}

std::string Message::getContent()
{
    return content_;
}
