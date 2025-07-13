//
// Created by Denis on 12.07.2025.
//

#include "StatementParser.h"
#include "Statement.h"
#include "token/Token.h"
#include "Parser.h"
#include <memory>

// реализация этого класса не оправдала себя из-за большей нагрузки архитектуры на данный момент
// всё, что предоставлял этот класс, можно реализовать в Parser
// под этим имеется в виду обработка if и { блоков кода }