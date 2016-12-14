#include "LocalHistory.h"

void SaveMessageStatuses(std::ofstream& file, const std::unordered_map<messenger::MessageId, messenger::message_status::Type>& statuses)
{
	
}

void SaveCurrentChats(std::ofstream& file, const std::unordered_map<messenger::UserId, std::vector<messenger::Message> >& chat)
{
	
}

void SaveNewMessages(std::ofstream& file, const std::unordered_map<messenger::UserId, std::vector<messenger::Message> >& statuses)
{
	
}

void LoadMessageStatuses(std::ifstream& file, std::unordered_map<messenger::MessageId, messenger::message_status::Type>& statuses)
{
	
}

void LoadCurrentChats(std::ifstream& file, std::unordered_map<messenger::UserId, std::vector<messenger::Message> >& chat)
{
	
}

void LoadNewMessages(std::ifstream& file, std::unordered_map<messenger::UserId, std::vector<messenger::Message> >& statuses)
{
	
}
