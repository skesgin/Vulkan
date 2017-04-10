#pragma once

#define REFLECT_TOKEN(Token) Token


#define VALIDATE_CONDITION_WITH_DEFAULT(Condition, Message, Default) if (REFLECT_TOKEN(Condition)) { vkts::logPrint(VKTS_LOG_ERROR, __FILE__, __LINE__, #Message); return REFLECT_TOKEN(Default); }

#define VALIDATE_CONDITION(Condition, Message) VALIDATE_CONDITION_WITH_DEFAULT(Condition, Message, VK_FALSE)


#define VALIDATE_SUCCESS_WITH_DEFAULT(Result, Message, Default) if (Result != VK_SUCCESS) { vkts::logPrint(VKTS_LOG_ERROR, __FILE__, __LINE__, #Message); return REFLECT_TOKEN(Default); }

#define VALIDATE_SUCCESS(Result, Message) VALIDATE_SUCCESS_WITH_DEFAULT(Result, Message, VK_FALSE)


#define VALIDATE_INSTANCE_WITH_DEFAULT(Ptr, Message, Default) 	if (!REFLECT_TOKEN(Ptr).get()) { vkts::logPrint(VKTS_LOG_ERROR, __FILE__, __LINE__, #Message); return REFLECT_TOKEN(Default); }

#define VALIDATE_INSTANCE(Ptr, Message) VALIDATE_INSTANCE_WITH_DEFAULT(Ptr, Message, VK_FALSE)