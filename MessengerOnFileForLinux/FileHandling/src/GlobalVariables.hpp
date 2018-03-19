#pragma once
#include <string>
#include <signal.h> //to wszystko ponizej potrzebne do initu dzialania sygnalu
#include <unistd.h>
#include <stdio.h>
#include <ctime>
#include <iostream>

#include <ChatRequest.hpp>

namespace ENIVRONMENTAL_PATH
{
    namespace PATH_TO_FILE
    {
        static constexpr auto REGISTERED_FILE= "/home/messenger/configuration/REGISTERED.txt";
        static constexpr auto LOGGED_FILE = "/home/messenger/configuration/LOGGED.txt";
    }

    namespace PATH_TO_FOLDER
    {
        static const std::string HOME_FOLDER = getenv("HOME");
        static const std::string USER_FOLDER = HOME_FOLDER + "/messenger/";
        static constexpr auto CHATS_FOLDER = "/home/messenger/chats/";
        static constexpr auto INSTALL_FOLDER = "/home/messenger/installation/";
    }
}

namespace FileStructure {

    namespace FileField
    {
        static constexpr int usernameFieldInLoggedFile = 0;
        static constexpr int statusFieldInLoggedFile = 1;
        static constexpr int pidFieldInLoggedFile = 2;
        static constexpr int usernameFieldInRegisteredFile = 0;
        static constexpr int passwordFieldInRegisteredFile = 1;
        static constexpr int dateTimeFieldInRegisteredFile = 2;
    }

    namespace FieldValue
    {
        static const std::string userActiveStatus = "0";
        static const std::string userBussyStatus = "1";
    }
}


/** To do jakiegos inita ***********************************/

static void sigusr1Handler(int sig_num, siginfo_t *info, void *context)
{
    if (NULL == info)
    {
        std::cerr << "WARNING: info = NULL" << std::endl;
    }
    ChatRequest chatRequest;
    chatRequest.answerForChatRequest(info -> si_pid);
}

static void initSigusr1Action()
{
    sigset_t iset;
    struct sigaction act;
    sigemptyset(&iset);

    act.sa_sigaction = sigusr1Handler;
    act.sa_mask = iset;
    act.sa_flags |= SA_SIGINFO;

    sigaction(SIGUSR1, &act, NULL);
}

/** **********************************************************/


