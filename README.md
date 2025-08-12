# IMGVIEW 2

__WIP:__ Not all imgview feature is implemented yet! (be patient)

a second iteration for [imgview](https://github.com/commrade-goad/imgview)
that written in CPP to decrease the amount of `SEGFAULT` imgview has and lazy loading the next image.

## Feature that different from imgview 1.0

- No more segfault! 😀
    * The amount of if NULL is unbelivable on the first imgview (yes i have a really big skill issue there)
- Lazy loading (max 5 threads) 💅
    * Will lazy load the image with max 5 threads so no more slow startup!
- Less image support for now 😔
    * Since this version used `stb_image` it will support less image format for now... (will upgrade in the future!)

## Deps
- SDL3

## Build
1. Generate build directory

```sh
meson setup build --buildtype=release # or debug
```

2. Build the project
```sh
meson compile -C build
```

3. Use it.
