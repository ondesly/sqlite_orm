// swift-tools-version: 5.9
// The swift-tools-version declares the minimum version of Swift required to build this package.

import PackageDescription

let package = Package(
    name: "sqlite_orm",
    products: [
        .library(name: "sqlite_orm", targets: ["c_sqlite_orm"]),
    ],
    dependencies: [
        .package(url: "https://github.com/ondesly/sqlite3.git", exact: "3.50.4")
    ],
    targets: [
        .target(
            name: "c_sqlite_orm",
            dependencies: [
                "sqlite3"
            ],
            path: "src",
            sources: [
                "sqlite_orm/aasset_vfs.cpp"
            ],
            publicHeadersPath: "include"
        )
    ],
    cxxLanguageStandard: .cxx17
)
