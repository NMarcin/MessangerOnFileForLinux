#include<TerminalControl.hpp>

#include <ChatRequest.hpp>
#include <ConversationControl.hpp>
#include <GlobalVariables.hpp>

#include <iostream>
#include <memory>

#include <iostream>
#include <chrono>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <streambuf>
#include <sstream>

using namespace std::chrono_literals;



TerminalControl::TerminalControl(ChatStatus chatStatus, std::shared_ptr<ChatInformation> chatInfo)
                                : chatStatus_(chatStatus)
                                , chatInfo_(chatInfo)
{
    // NOOP
}

TerminalControl::TerminalControl(ChatStatus chatStatus)
                                : chatStatus_(chatStatus)
{
    //NOOP
}


class zeroNumerator: public std::exception
{
    const char* what() const throw() { return "Numerator can't be 0.\n"; }
};


bool TerminalControl::waitingInTerminal()
{

    log_.function("TerminalControl::waitingInTerminal()");
    char command[512];
    getstr(command);
    log_.function((std::string(command) + "|").c_str());
    if ("start oskierko" == std::string(command))
    {
        log_.function("TerminalControl::waitingInTerminal() START");
        return startConversationAsRecipient("oskierko");
    }

    bool commandStatus = terminalFunctionality_.runCommand(std::string(command), chatInfo_);

    return commandStatus;

}

bool TerminalControl::startConversation(const std::string& username, ChatRole chatRole)
{
    log_.function(("ChatControl::startConversation started whit chatRole = " + std::to_string(static_cast<int>(chatRole))).c_str());
    if (ChatRole::inviter == chatRole)
    {
        return startConversationAsInviter(username);
    }
    else if (ChatRole::recipient == chatRole)
    {
        return startConversationAsRecipient(username);
    }
}

bool TerminalControl::startConversationAsInviter(const std::string& username)
{
    log_.function("TerminalControl::startConversationAsInviter()");
    ChatRequest chatRequest;
    chatFileWithPath_ = chatRequest.sendChatRequest(username);
    if(!chatFileWithPath_.empty())
    {
        std::string info = "ChatControl::startConversationAsInviter chatFileWithPath_: " + chatFileWithPath_;
        if (nullptr == chatInfo_)
        {
            log_.info("DUPAAAAA");
        }
        log_.info(info.c_str());
        log_.info("INVITER PRZED");
        chatInfo_->chatPath_ = chatFileWithPath_;
        log_.info("CZAT PATH UPDATE");
        chatInfo_->messageFlag_ = MessageFlag::inviterMessage;
        log_.info("FLAG UPDATE");
        return true;

        //ConversationControl conversation(chatFileWithPath_, messageFlag_);
        //conversation.conversation();
    }
    return false;
}

bool TerminalControl::startConversationAsRecipient(const std::string& username)
{
    log_.function("TerminalControl::startConversationAsRecipient()");
    ChatRequest chatRequest;
    chatFileWithPath_ = chatRequest.answerForChatRequest(username, "dupa");
    if(!chatFileWithPath_.empty())
    {
        std::string info = "ChatControl::startConversationAsRecipient chatFileWithPath_: " + chatFileWithPath_;
        log_.info(info.c_str());
        log_.info("RECEIVER PRZED");
        chatInfo_->chatPath_ = chatFileWithPath_;
        log_.info("RECEIVER PRZED");
        chatInfo_->messageFlag_ = MessageFlag::recipientMessage;
        return true;

        //ConversationControl conversation(chatFileWithPath_, messageFlag_);
        //conversation.conversation();
    }
    return false;
}


void TerminalControl::lookForInvitation()
{
    while (isMessengerRunnig)
    {
        auto invitationsFolderContent = FileInterface::Accesor::getFilenamesFromFolder(ENVIRONMENT_PATH::TO_FOLDER::INVITATIONS);
        if (0 == invitationsFolderContent->size())
        {
            sleep(1);
            continue;
        }
        for(auto invitation : *invitationsFolderContent)
        {
            std::string recipent;
            std::string inviter;
            bool getRecipentName = true;

            for (auto letter : invitation)
            {
                if (!getRecipentName)
                {
                    inviter += letter;
                }

                else if ('_' != letter)
                {
                    recipent += letter;
                }

                else if ( '_' == letter)
                {
                    getRecipentName = false;
                }

            }

            if (recipent == getenv("USER"))
            {
                clear();
                printw(("You get an invitation to chat form " + inviter + "\n").c_str());
                printw("Do you want to chat with this user (y/n)? \n");
                printw(">> ");
                refresh();
                //TerminalControl terminalControl(ChatStatus::terminal);
                //terminalControl.startConversation(inviter, ChatRole::recipient);
            }
        }
        //sleep(1);
        isMessengerRunnig=false;
    }
}
