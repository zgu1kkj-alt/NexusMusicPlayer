#pragma once
#include <QString>
class SearchManager { public: static QString normalize(const QString &text) { return text.trimmed(); } };
