#pragma once

#include <Core.h>

class Watchdog
{
public:

  Watchdog(std::filesystem::path const& filePath, std::string const& fileExtension);

public:

  void Update();

  inline std::set<std::filesystem::path> const& FilesToCreate() const { return mFilesToCreate; }
  inline std::set<std::filesystem::path> const& FilesToDelete() const { return mFilesToDelete; }
  inline std::set<std::filesystem::path> const& FilesToChange() const { return mFilesToChange; }

private:

  void CheckDeletedFiles();
  void CheckInsertedFiles();
  void CheckChangedFiles();

private:

  std::filesystem::path mFolder = {};
  std::filesystem::path mExtension = {};

  std::map<std::filesystem::path, std::filesystem::file_time_type> mFileInfos = {};

  std::set<std::filesystem::path> mFilesToCreate = {};
  std::set<std::filesystem::path> mFilesToDelete = {};
  std::set<std::filesystem::path> mFilesToChange = {};
};