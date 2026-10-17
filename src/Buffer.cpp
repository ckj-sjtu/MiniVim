#include <fstream>

#include "Buffer.hpp"

namespace sjtu {

Buffer::Buffer(const std::filesystem::path& path):path_{path}{
    //从path指向的文件构造Buffer,你需要打开文件并且把文件内容填充进Buffer,并正确初始化一些状态.
    //注意path可能为空的边界情况
    if(!path.empty()){
        std::ifstream in(path, std::ios::binary);
        if(in){
            std::string line;
            while(std::getline(in, line)){
                if(!line.empty() && line.back() == '\r') line.pop_back();
                lines_.push_back(std::move(line));
            }
        }
    }
    if(lines_.empty()) lines_.emplace_back();
}

Buffer::Buffer(std::vector<std::string> lines, std::filesystem::path path):lines_{std::move(lines)}, path_{std::move(path)} {
    if(lines_.empty()) lines_.emplace_back();
}

std::size_t Buffer::GetLineCount() const {
    //返回文件行数
    return lines_.size();
}

const std::string& Buffer::GetLineAt(std::size_t row) const {
    //返回第row行的内容
    return lines_[row];
}


std::string Buffer::GetDisplayName() const {
    //返回文件名,若是新文件,返回"[No Name]"
    if(path_.empty()) return "[No Name]";
    return path_.filename().string();
}

bool Buffer::IsModified() const {
    //返回文件和上次保存比起来是否被修改过
    return modified_;
}

void Buffer::InsertCharacter(std::size_t row, std::size_t column, char value) {
    //在第row行第col列插入一个value, 注意越界检查
   if(lines_.size() > row && lines_[row].size() >= column){
    (lines_[row]).insert(column, 1, value); 
    modified_ = true;
   } 
}

void Buffer::EraseCharacter(std::size_t row, std::size_t column) {
   //在第row行第col列删除一个value
   if(lines_.size() > row && lines_[row].size() > column){
    lines_[row].erase(column);
    modified_ = true;
   } 
}

void Buffer::SplitLine(std::size_t row, std::size_t column) {
    //在第row行第col列分割,即在此处敲了回车键
    if(lines_.size() > row && lines_[row].size() >= column){
        std::string& line{lines_[row]};
        std::string  line2{line.substr(column)};
        line = line.substr(0, column);
        lines_.insert(lines_.begin() + row + 1, line2);
        modified_ = true;
    }
    
}

void Buffer::JoinLine(std::size_t row) {
   //把第row + 1行合并进第row行
   if(lines_.size() > row + 1){
    std::string line{lines_[row + 1]};
    lines_[row].insert(lines_[row].size(), line);
    lines_.erase(lines_.begin() + row + 1);
    modified_ = true;
   }
}

void Buffer::Save() {
   //把文件内容保存, 直接调用WriteTo方法
   if(!path_.empty()){
    WriteTo(path_);
    modified_ = false;
   }
   
}

void Buffer::SaveAs(const std::filesystem::path& path) {
    WriteTo(path);
    path_ = path;
    modified_ = false;
}


void Buffer::WriteTo(const std::filesystem::path& path) const {
   //实际将缓冲区中的内容写入path指向的文件中
   std::ofstream out_file(path);
   if(!out_file.is_open()) return;
   for(std::size_t i = 0; i < lines_.size(); i++){
    out_file << lines_[i];
    if(i < lines_.size() - 1) out_file << '\n';
   }
}

} // namespace sjtu
