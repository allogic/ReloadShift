#include <Filesystem/Watchdog.h>

Watchdog::Watchdog(std::filesystem::path const& filePath, std::string const& fileExtension)
  : mFolder{ filePath }
  , mExtension{ fileExtension }
{
  std::filesystem::create_directory(mFolder);
}

void Watchdog::Update()
{
  CheckDeletedFiles();
  CheckChangedFiles();
  CheckInsertedFiles();
}

void Watchdog::CheckInsertedFiles()
{
  mFilesToCreate.clear();
  for (const auto& file : std::filesystem::directory_iterator(mFolder))
  {
    if (file.path().extension() != mExtension) continue;
    if (mFileInfos.find(file.path()) != mFileInfos.cend()) continue;
    mFileInfos.emplace(file.path(), std::filesystem::last_write_time(file.path()));
    mFilesToCreate.emplace(file.path());
  }
}
void Watchdog::CheckDeletedFiles()
{
  mFilesToDelete.clear();
  std::erase_if(mFileInfos, [&](const auto& fileInfo)
  {
    const auto& file = fileInfo.first;
    const auto exists = std::filesystem::exists(file);
    if (!exists) mFilesToDelete.emplace(file);
    return !exists;
  });
}
void Watchdog::CheckChangedFiles()
{
  mFilesToChange.clear();
  for (auto& [file, prevTime] : mFileInfos)
  {
    auto const time = std::filesystem::last_write_time(file);
    if (time > prevTime)
    {
      prevTime = time;
      mFilesToChange.emplace(file);
    }
  }
}