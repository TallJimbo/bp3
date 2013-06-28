import sys
if sys.version_info[0] > 2:
    import builtins as exceptions
else:
    import exceptions

def get_builtin_exceptions():
    """Return a list of all builtin exception classes, sorted to resolve
    dependencies.
    """
    classes = list()
    unsorted = set()
    for name in dir(exceptions):
        cls = getattr(exceptions, name)
        if isinstance(cls, type) and issubclass(cls, BaseException):
            unsorted.add(cls)
    unsorted.remove(BaseException)
    classes = [BaseException]
    while unsorted:
        current = unsorted.pop()
        to_add = []
        while current not in classes:
            to_add.append(current)
            current = current.__base__
        for cls in to_add[::-1]:
            classes.append(cls)
            unsorted.discard(cls)
    return classes
