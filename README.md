V A L E N T I N E
==================

A compressor that's meant to make tracks pump and
breathe.

This is an incredibly in progress repository. 

The intention is for it to be open source—and not just that,
but a resource for people who want to get into audio software
development, and plugin development, but aren't quite sure how.

If you're here, it's probably because the downloads are hosted here.
It's definitely in a state good enought to start sharing. However,
the code itself is often an example of what not to do. So I'll ask
you to avert your eyes for now.  

Building Valentine
==================

Xcode
-----

Currently, `Xcode 14.2` is explicitly supported for building Valentine.
Here are the steps needed to create an Xcode project for building the plugin.

```
git clone https://github.com/Tote-Bag-Labs/valentine.git
cd valentine
git submodule update --init --recursive
cmake -B builds -G Xcode
```

Other (for now)
---------------

If you'd rather just build it from the command line, run
`cmake --build Builds --config Release`
