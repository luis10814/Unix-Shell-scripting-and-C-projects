BEGIN {
    package = ""
}

/^pkg / {
    package = $2
}

/^dep / {
    if (package == wanted) {
        print $2
    }
}
