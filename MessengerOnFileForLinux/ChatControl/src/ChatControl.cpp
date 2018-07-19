#include <chrono>
#include <thread>

#include <ChatControl.hpp>
#include <FileHandling.hpp>
#include <ChatRequest.hpp>
#include <GlobalVariables.hpp>
#include <LocalUser.hpp>
#include <Display.hpp>

ChatControl::ChatControl()
{
    log.info("ChatControl C-TOR ");
    getMessageToQueueThread_ = nullptr;
    reciveMessageThread_ = nullptr;
    sendMessageFromQueueThread_ = nullptr;
}

ChatControl::~ChatControl()
{
    log.info("ChatControl D-TOR ");

    if (getMessageToQueueThread_ && reciveMessageThread_)
    {
        getMessageToQueueThread_->join();
        reciveMessageThread_->join();
        sendMessageFromQueueThread_->join();
    }
}

void ChatControl::conversationProlog(const std::string& username, ChatRole chatRole)
{
    log.info(("ChatControl::startConversation started whit chatRole = " + std::to_string(static_cast<int>(chatRole))).c_str());
    if (ChatRole::inviter == chatRole)
    {
        startConversationAsInviter(username);
    }
    else if (ChatRole::recipient == chatRole)
    {
        startConversationAsRecipient(username);
    }
}

void ChatControl::conversationEpilog()
{
    log.info("ChatControl::endConversation started");
    stopThreads();
    delwin(enterMessageWindow_);
    delwin(displayMessageWindow_);
    //Display::displayMainWindow();
    //refresh();
    //TODO mawoznia dalsza czesc konczenia rozmowe, pobieranie historii itd.
    //Zmiana statusów
    //Rozdzielic na dwie sytuacje, z bledem i bez
    //Jak poinformowac druga osobe ze kniec ? wyslac jej //unexpected_end ?
    const std::string userActiveStatus = "0";
    const std::string username = LocalUser::getLocalUser().getUsername();
    FileInterface::Modification::updateRowField(ENVIRONMENT_PATH::TO_FILE::LOGGED_FILE, username, userActiveStatus, FileStructure::FileField::statusFieldInLoggedFile);
}

void ChatControl::stopThreads()
{
    isThreadsRunning_ = false;
}

void ChatControl::getMessage()
{
    log.info("ChatControl::getMessage started");
    std::unique_ptr<Sender> sender = std::make_unique<Sender>(chatFileWithPath_, static_cast<int>(messageFlag_), enterMessageWindow_);
    while(isThreadsRunning_)
    {
        Display::displayEnterMessageWindow(enterMessageWindow_);
        messageWaitingRoom_.push(sender->getMessageToSend());

        //TODO mawoznia Wersja do testownia. potem zamienic na wiadomosci z implementacji mnurzyn
        auto tmp = *messageWaitingRoom_.front();
        messageToDisplay_.push(tmp);
    }
}

void ChatControl::reciveMessage()
{
    log.info("ChatControl::reciveMessage started");
    std::unique_ptr<Receiver> receiver = std::make_unique<Receiver>();

    while(isThreadsRunning_)
    {
        sleep(1);
        //reciver->recive()
        if (!messageToDisplay_.empty())
        {
            std::string message = *FileInterface::Accesor::getRowField(messageToDisplay_.front(), 3);

            if (5 <= message.size() && "//end" == std::string(message.begin(), message.begin()+5))
            {
                //TODO mawoznia narzucic tu implementacje z terminal funcionality
                log.info("ChatControl::reciveMessage Receive 'end conversatoin' request");
                messageToDisplay_.pop();
                conversationEpilog();
            }
            else if (2 <= message.size() && "//" == std::string(message.begin(), message.begin()+2))
            {
                log.info("ChatControl::reciveMessage Receive 'terminal funcionality' request");
                //TODO mawoznia co chcemy jeszcze wywolywac bezposrednio z czatu ?
                messageToDisplay_.pop();
            }
            else
            {
                Display::displayDisplayMessageWindow(displayMessageWindow_, messageToDisplay_.front() + "\n");
                messageToDisplay_.pop();
            }
        }
    }
}

void ChatControl::sendMessage()
{
    log.info("ChatControl::sendMessage started");
    std::unique_ptr<Sender> sender = std::make_unique<Sender>(chatFileWithPath_, static_cast<int>(messageFlag_), enterMessageWindow_);
    while(isThreadsRunning_ || !messageWaitingRoom_.empty())
    {
        if (messageWaitingRoom_.empty())
        {
            sleep(1);
        }
        else
        {
            sender->sendMessage(*messageWaitingRoom_.front());
            messageWaitingRoom_.pop();
        }
    }
}

void ChatControl::startConversationAsInviter(const std::string& username)
{
    log.info("ChatControl::startConversationAsInviter started");
    ChatRequest chatRequest;
    chatFileWithPath_ = chatRequest.sendChatRequest(username);
    if(!chatFileWithPath_.empty())
    {
        std::string info = "ChatControl::startConversationAsInviter chatFileWithPath_: " + chatFileWithPath_;
        log.info(info.c_str());
        messageFlag_ = MessageFlag::inviterMessage;
        conversation();
    }
    else
    {
        conversationEpilog();
    }
}

void ChatControl::startConversationAsRecipient(const std::string& username)
{
    log.info("ChatControl::startConversationAsRecipient started");
    ChatRequest chatRequest;
    chatFileWithPath_ = chatRequest.answerForChatRequest(username);
    if(!chatFileWithPath_.empty())
    {
        std::string info = "ChatControl::startConversationAsRecipient chatFileWithPath_: " + chatFileWithPath_;
        log.info(info.c_str());
        messageFlag_ = MessageFlag::recipientMessage;
        conversation();
    }
    else
    {
        conversationEpilog();
    }
}

void ChatControl::conversation()
{
    clear(); //TODO mwoznia czemu jak opakuje 150-165 w funkjce to program sie sypie
    int x, y;
    getmaxyx(stdscr, y, x);
    displayMessageWindow_ = newwin(y * 0.75, x, 1, 1);//size y,x; wspolrzedne startu
    enterMessageWindow_ = newwin(y * 0.25 ,x, y * 0.8 + 1 ,1);

    std::string frame;
    for (int i = 0; i < x; i++)
    {
        frame += "-";
    }

    wprintw(enterMessageWindow_, frame.c_str());
    wrefresh(displayMessageWindow_);
    wrefresh(enterMessageWindow_);
    refresh();
    isThreadsRunning_ = true;

    log.info("ChatControl::conversationControl started");
    getMessageToQueueThread_ = std::make_unique<std::thread>(std::thread([&]()
    {
        getMessage();
    }));

    reciveMessageThread_ = std::make_unique<std::thread>(std::thread([&]()
    {
        reciveMessage();
    }));

    sendMessageFromQueueThread_ = std::make_unique<std::thread>(std::thread([&]()
    {
        sendMessage();
    }));

}
