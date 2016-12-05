## Reduce performance in Swift




Requirement: Swift 3.0 or better.

```
$ swift build --configuration release
$ ./.build/release/reduce
(0.4, 0.6, 0.19, 0.55)
```

## For Xcode users (Mac only)

```
$  swift package generate-xcodeproj
generated: ./reduce.xcodeproj
$ open ./reduce.xcodeproj
```

You can also profile the application:
```
$ iprofiler -timeprofiler ./.build/release/reduce
```
Then open the newly created file with Instrument.

