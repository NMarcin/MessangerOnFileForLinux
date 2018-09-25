#pragma once
#include <stack>
#include <queue>
#include <string>
#include <memory>

#include <Logger.hpp>
#include <LogSpace.hpp>
#include <PurgeMessage.hpp>
/*
- pobieramy cały plik
- blokujemy dostęp do pliku
-

- pobieramy od końca pliku linijki które mają odpowiednią flagę na początku
- uzywamy do tego stosu, bo potem w odwrotnej kolejnosci bedziemy chcieli wrzucac do kolejki
- nastepnie z tego stosu zabieram po linijce i oprawiamy ja i zwracamy w innej funkcji
- kolejna funkcja wrzuca oprawiona linijke do naszej kolejki do wyswietlania
*/

class Receiver
{
public:
    Receiver(std::string chatFileWithPath, std::string mineMessageUserFlag);      // chatFileWithPath -> do konstruktora
    ~Receiver() = default;

    bool readMessagesToStack();
    std::unique_ptr<PurgeMessage> returnTheOldestMessage();

private:
    bool updateSeenFlags();
    bool endOfMessageToRead(std::string message, std::string messageFlag);
    bool isChatFileEmpty(std::unique_ptr<std::vector<std::string>>& chatFileContent);
    void pushPurgeMessageOnStack(std::string rawMessageToPush);
    std::unique_ptr<PurgeMessage> messagePurging(Message& messageToPurge);
    void removeFlagNEW();

    const std::string chatFileWithPath_;
    std::stack<PurgeMessage> purgeMessagesStack_;
    std::string mineMessageUserFlag_;

    Logger log_{LogSpace::ChatControl};
};
