/*
 * Copyright 2019 The Chromium Authors. All rights reserved.
 * Use of this source code is governed by a BSD-style license that can be
 * found in the LICENSE file.
 */

/**
 * Launch PaymentRequest with a show promise that resolve with invalid details.
 * @param {string} supportedMethods The payment method that is supported by this
 *        request.
 */
function buyWithMethods(supportedMethods) { // eslint-disable-line no-unused-vars, max-len
  try {
    new PaymentRequest([{supportedMethods}], {
      total: {
        label: 'PENDING TOTAL',
        amount: {currency: 'USD', value: '99.99'},
      },
    })
        .show(new Promise(function(resolve) {
          resolve({
            total: {
              label: 'Total',
              amount: {currency: 'USD', value: '-1.00'},
            },
          });
        }))
        .catch(function(error) {
          print(error);
        });
  } catch (error) {
    print(error.message);
  }
}
