(append
  (list
    (channel
      (name 'stables)
      (url "https://git.cmmshq.ru/git/infrastructure/stables.git")
      (introduction
        (make-channel-introduction
          "da6207c81eba98dc0e1ee06e48465eaa194977af"
          (openpgp-fingerprint "A67E AEA7 1110 F2B1 4C06  A5AB 2650 9599 1AC5 7411"))))
    (channel
      (name 'nonguix)
      (url "https://git.cmmshq.ru/git/infrastructure/nonguix.git")
      ;; linux 5.7
      (commit "95adb6ec077dc4139cf7e1956ad5caa1ff08f92b")
      (introduction
        (make-channel-introduction
          "897c1a470da759236cc11798f4e0a5f7d4d59fbc"
          (openpgp-fingerprint "2A39 3FFF 68F4 EF7A 3D29  12AF 6F51 20A0 22FB B2D5"))))
    (channel
      (name 'guix)
      (url "https://git.cmmshq.ru/git/infrastructure/guix.git")
      ;; linux 5.7
      (commit "6b44b35a57d688da3791c08d65f27b6677da7b1f")
      (introduction
        (make-channel-introduction
          "9edb3f66fd807b096b48283debdcddccfea34bad"
          (openpgp-fingerprint "BBB0 2DDF 2CEA F6A8 0D1D  E643 A2A0 6DF2 A33A 54FA"))))))
