# Aliases source ~/.bashrc

# alias alias_name="command_to_run"

# Firmware Aliases
alias del-sdkconfig='rm sdkconfig'
alias make-debug='cmake --preset "Debug"'
alias make-jtag='cmake --preset "Debug JTAG"'
alias make-tests='cmake --preset "Tests"'
alias make-release='make --preset "Release"'
alias make-build='cmake --build build --parallel'
alias make-zip='cmake --build build --target FirmwareRelease'
alias make-flash='idf.py flash'
alias make-appflash='idf.py app-flash'
alias make-flash-monitor='idf.py flash monitor'
alias make-appflash-monitor='idf.py app-flash monitor'
alias make-fullclean='idf.py fullclean'
alias make-monitor='idf.py monitor'
alias make-reconfigure='idf.py reconfigure'
alias make-eraseflash='idf.py erase_flash'
alias make-all-build='del-sdkconfig; make-fullclean; make-release; make-build'
alias make-all-zip='del-sdkconfig; make-fullclean; make-release; make-zip'

# Own Firmware Functions
alias make-release-local='cmake --preset "ReleaseLocal"'
alias make-all-build-local='del-sdkconfig; make-fullclean; make-release-local; make-build'
alias make-all-zip-local='del-sdkconfig; make-fullclean; make-release-local; make-zip'
