// META: global=dedicatedworker-module,sharedworker-module,serviceworker-module

import { importMetaOnRootModule, importMetaOnDependentModule }
    from "./import-meta-root.js";

const base = location.href.slice(0, location.href.lastIndexOf('/'));

test(() => {
  assert_equals(importMetaOnRootModule.url,
                base + "/import-meta-root.js");
}, "import.meta.url in a root external script");

test(() => {
  assert_equals(importMetaOnDependentModule.url,
                base + "/import-meta-dependent.js");
}, "import.meta.url in a dependent external script");

test(() => {
  assert_equals(typeof importMetaOnRootModule, "object");
  assert_not_equals(importMetaOnRootModule, null);
}, "import.meta is an object");

test(() => {
  importMetaOnRootModule.newProperty = 1;
  assert_true(Object.isExtensible(importMetaOnRootModule));
}, "import.meta is extensible");

test(() => {
  const names = new Set(Reflect.ownKeys(importMetaOnRootModule));
  for (const name of names) {
    var desc = Object.getOwnPropertyDescriptor(importMetaOnRootModule, name);
    assert_equals(desc.writable, true);
    assert_equals(desc.enumerable, true);
    assert_equals(desc.configurable, true);
  }
}, "import.meta's properties are writable, configurable, and enumerable");


import { importMetaOnRootModule as hashedImportMetaOnRootModule1,
         importMetaOnDependentModule as hashedImportMetaOnDependentModule1 }
       from "./import-meta-root.js#1";

import { importMetaOnRootModule as hashedImportMetaOnRootModule2,
         importMetaOnDependentModule as hashedImportMetaOnDependentModule2 }
       from "./import-meta-root.js#2";

test(() => {
  assert_equals(hashedImportMetaOnRootModule1.url,
                base + "/import-meta-root.js#1");
  assert_equals(hashedImportMetaOnRootModule2.url,
                base + "/import-meta-root.js#2");

  // Must not be affected
  assert_equals(hashedImportMetaOnDependentModule1.url,
                base + "/import-meta-dependent.js");
  assert_equals(hashedImportMetaOnDependentModule2.url,
                base + "/import-meta-dependent.js");
}, "import.meta.url when importing the module with different fragments");
