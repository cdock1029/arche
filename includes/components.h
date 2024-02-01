#pragma once

//                _                  https://github.com/rthrfrd/webxx
//  __      _____| |____  ___  __
//  \ \ /\ / / _ | '_ \ \/ \ \/ /    MIT License
//   \ V  V |  __| |_) >  < >  <     Copyright (c) 2022
//    \_/\_/ \___|_.__/_/\_/_/\_\    Alexander Carver
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
// SOFTWARE.

#include "database.h"
#include "htmx.h"
#include <QJsonArray>
#include <QJsonObject>
#include <webxx.h>

namespace components {
constexpr auto PICO_URL = "https://unpkg.com/@picocss/pico@1.5.11/css/pico.min.css";
constexpr auto HTMX_URL = "https://unpkg.com/htmx.org@1.9.10";
constexpr auto HTMX_INTEGRITY = "sha384-D1Kt99CQMDuVetoL1lrYwg5t+9QdHe7NLX/SoJYkXDFfX37iInKRy5xLSi8nO7UC";
constexpr auto SHORTCUT_ICON = "https://www.svgrepo.com/show/46184/frog.svg";
// constexpr auto SWEET_ALERT_URL = "https://unpkg.com/sweetalert/dist/sweetalert.min.js";
constexpr auto SWEET_ALERT2_URL = "https://cdn.jsdelivr.net/npm/sweetalert2@11";

using namespace Webxx;
using namespace Qt::StringLiterals;

struct Main : component<Main> {
    explicit Main(node&& mainContent)
        : component<Main> {
            main {
                { _class { { "container" } } },
                nav {
                    { htmx::_hxBoost {} },
                    a { { _href { "/" }, _hxSelect { "#content" }, _hxTarget { "#content" } }, "Home" },
                    a { { _href { "/treasuries" }, _hxSelect { "#content" }, _hxTarget { "#content" } }, "Treasuries" },
                },
                hr {},
                dv {
                    { _id { "content" } },
                    std::move(mainContent),
                } }
        }
    {
    }
};

struct Layout : component<Layout> {
    explicit Layout(node&& mainComp)
        : component<Layout> {
            doc {
                html {
                    head {
                        title { "Arche" },
                        Webxx::link { { _rel { "shortcut icon" }, _href { SHORTCUT_ICON } } },
                        Webxx::link { { _rel { "stylesheet" }, _href { PICO_URL } } },
                        style {
                            { "tbody button", { marginBottom { "0" } } },
                            { "td form", display { "none" } } },
                        styleTarget {},
                        headTarget {},
                        Webxx::script {
                            { _src { HTMX_URL }, _integrity { HTMX_INTEGRITY }, _crossorigin { "anonymous" } },
                        },
                        Webxx::script {
                            { _src { SWEET_ALERT2_URL } },
                        },
                        script { R"(
                            htmx.config.useTemplateFragments = true;
                            htmx.on("htmx:responseError", function (evt) {
                                console.log("htmx:responseError : ", evt.detail.xhr);
                                let error = evt.detail.xhr.responseText;
                                if (error) {
                                    SwalError(error);
                                }
                            });
                            /*
                            document.addEventListener("htmx:confirm", function(e) {
                                e.preventDefault();
                                console.log("in htmx:confirm");
                                if (!e.target.hasAttribute('hx-confirm')) {
                                    e.detail.issueRequest(true);
                                    return;
                                }
                                Swal.fire({
                                    title: "Confirm?",
                                    text: `${e.detail.question}`,
                                    icon: "warning",
                                    showCancelButton: true,
                                    confirmButtonText: "Yes",
                                    cancelButtonText: "No",
                                    reverseButtons: true
                                }).then(function(result) {
                                    if(result.isConfirmed) e.detail.issueRequest(true); // use true to skip window.confirm
                                });
                            });
                            */
                            function SwalSuccess(text, title = "Success") {
                                Swal.fire({title:title,text:text,icon:"success",allowOutsideClick:false});
                            }
                            function SwalError(text, title = "Error") {
                                Swal.fire({title:title,text:text,icon:"error",allowOutsideClick:false});
                            }
                            )" },
                    },
                    body {
                        std::move(mainComp),
                    } } }
        }
    {
    }
};

struct Home : component<Home> {
    Home()
        : component<Home> {
            Main {
                fragment {
                    h1 { "Home" },
                    form {
                        {
                            _hxPost { "/search" },
                            _hxTarget { "#result" },
                            htmx::_hxSwap { htmx::Swap::InnerHtml },
                            htmx::_hxResetAfterSuccess {},
                        },
                        input {
                            { _type { "search" }, _name { "cusip" }, _placeholder { "CUSIP" } } },
                    },
                    dv {
                        { _id { "result" } } } } }
        }
    {
    }
};

struct Treasuries : component<Treasuries> {
    Treasuries(const QList<arche::data::Ust>& treasuries)
        : component<Treasuries> {
            Main {
                fragment {
                    h1 { "Treasuries" },
                    table {
                        thead {
                            th { "CUSIP" },
                            th { "Issue Date" },
                            th { "Maturity Date" },
                            th { "Week/Year Term" },
                            th { "Day/Month Term" },
                            th {} },
                        tbody {
                            {
                                _hxConfirm { "Are you sure?" },
                                _hxTarget { "closest tr" },
                                htmx::_hxSwap { htmx::Swap::Delete },
                                _hxOnAfterRequest { "SwalSuccess('UST Deleted','Confirmation')" },
                            },
                            loop(treasuries, [](const arche::data::Ust& ust, [[maybe_unused]] const Loop& loop) {
                                return tr {
                                    { _id { u"row_%1"_s.arg(loop.index).toStdString() } },
                                    td {
                                        ust.cusip.toStdString() },
                                    td {
                                        ust.issueDate.toString().toStdString() },
                                    td { ust.maturityDate.toString().toStdString() },
                                    td { ust.securityTermWeekYear.toStdString() },
                                    td { ust.securityTermDayMonth.toStdString() },
                                    td {
                                        button {
                                            { _hxDelete { u"/treasuries/%1"_s.arg(loop.index).toStdString() } },
                                            "Delete" } },
                                };
                            }) },
                    } } }
        }
    {
    }
};

struct SearchResults : component<SearchResults> {
    SearchResults(const QJsonArray& arr)
        : component<SearchResults> {
            fragment {
                table {
                    thead {
                        th { "CUSIP" },
                        th { "Issue Date" },
                        th { "Maturity Date" },
                        th { "Week/Year Term" },
                        th { "Day/Month Term" },
                        th {} },
                    tbody {
                        loop(arr, [](QJsonValue&& v, [[maybe_unused]] const Loop& loop) {
                            return tr {
                                td {
                                    form {
                                        { _id { u"form_%1"_s.arg(loop.index).toStdString() }, _hxPost { "/" }, htmx::_hxSwap { htmx::Swap::None } },
                                        input {
                                            { _type { "hidden" }, _name { "cusip" }, _value { v[u"cusip"_s].toString().toStdString() } },
                                        },
                                    },
                                    v[u"cusip"_s].toString().toStdString() },
                                td {
                                    input { { _form { u"form_%1"_s.arg(loop.index).toStdString() }, _type { "hidden" }, _name { "issueDate" }, _value { v[u"issueDate"_s].toString().toStdString() } } },
                                    v[u"issueDate"_s].toString().toStdString() },
                                td { v[u"maturityDate"_s].toString().toStdString() },
                                td { v[u"securityTermWeekYear"_s].toString().toStdString() },
                                td { v[u"securityTermDayMonth"_s].toString().toStdString() },
                                td { button { { _form { u"form_%1"_s.arg(loop.index).toStdString() }, _type { "submit" } }, "Save" } }
                            };
                        }) } } },
        }
    {
    }
};
}
