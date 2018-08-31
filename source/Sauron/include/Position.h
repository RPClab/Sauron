 /*
 * Copyright (c) 2018 Lagarde François lagarde.at.sjtu.edu.cn
 * 
 * Permission is hereby granted, free of charge, to any person
 * obtaining a copy of this software and associated documentation
 * files (the "Software"), to deal in the Software without
 * restriction, including without limitation the rights to use,
 * copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the
 * Software is furnished to do so, subject to the following
 * conditions:
 * 
 * The above copyright notice and this permission notice shall be
 * included in all copies or substantial portions of the Software.
 * 
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES
 * OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
 * NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT
 * HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
 * WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
 * OTHER DEALINGS IN THE SOFTWARE.
 */

#ifndef POSITION_H
#define POSITION_H
#include <string>
#include "Value.h"
#include <ostream>
#include "ID.h"
class Position
{
public:
    bool operator==(const Position& pos) const;
    bool operator!=(const Position& pos) const;
    void setChannel(const Value& channel);
    void setModule(const Value& moduleName);
    void setCrate(const Value& crateName);
    void setRack(const Value& rackName);
    int getChannel() const;
    int getModule() const;
    int getCrate() const;
    int getRack() const;
    std::string getModuleName() const;
    std::string getCrateName() const;
    std::string getRackName() const;
    void  print(std::ostream& stream=std::cout,const std::string mover="");
private:
    friend std::ostream & operator<<(std::ostream &os, const Position& Pos);
    int m_channel{-1};
    int m_module{-1};
    int m_crate{-1};
    int m_rack{-1};
};
#endif
