#pragma once

#include <QHttpServerRequest>
#include <QHttpServerResponse>
#include <QObject>
#include <webxx.h>

// NOLINTBEGIN(*-avoid-c-arrays, *-pro-bounds-array-to-pointer-decay, *-no-array-decay)
namespace htmx {

/*
innerHTML - Replace the inner html of the target element
outerHTML - Replace the entire target element with the response
beforebegin - Insert the response before the target element
afterbegin - Insert the response before the first child of the target element
beforeend - Insert the response after the last child of the target element
afterend - Insert the response after the target element
delete - Deletes the target element regardless of the response
none- Does not append content from response (out of band items will still be processed).
*/
enum class Swap {
    InnerHtml,
    OuterHtml,
    BeforeBegin,
    AfterBegin,
    BeforeEnd,
    AfterEnd,
    Delete,
    None,
};

QHttpServerResponse&& hxRedirect(QHttpServerResponse&& resp, QByteArray&& path);
bool isHxRequest(const QHttpServerRequest& req);

struct _hxSwap : Webxx::internal::HtmlAttribute {
    explicit _hxSwap(Swap swap);
};

struct _hxBoost : Webxx::internal::HtmlAttribute {
    explicit _hxBoost(bool status = true);
};

struct _hxOn : Webxx::internal::HtmlAttribute {
    explicit _hxOn(QLatin1StringView event, QLatin1StringView js);
};

struct _hxOnAfterRequest : Webxx::internal::HtmlAttribute {
    explicit _hxOnAfterRequest(QLatin1StringView js);
};

struct _hxResetAfterSuccess : Webxx::internal::HtmlAttribute {
    explicit _hxResetAfterSuccess();
};

struct _hxOnAfterSwap : Webxx::internal::HtmlAttribute {
    explicit _hxOnAfterSwap(QLatin1StringView js);
};

}

namespace Webxx {
// @formatter:off
// clang-format off
WEBXX_HTML_ATTR_ALIAS(hx-post, hxPost);
WEBXX_HTML_ATTR_ALIAS(hx-delete, hxDelete);
WEBXX_HTML_ATTR_ALIAS(hx-trigger, hxTrigger);
WEBXX_HTML_ATTR_ALIAS(hx-target, hxTarget);
WEBXX_HTML_ATTR_ALIAS(hx-confirm, hxConfirm);
WEBXX_HTML_ATTR_ALIAS(hx-select, hxSelect);
WEBXX_HTML_ATTR_ALIAS(hx-on-htmx-before-request, hxOnBeforeRequest);
WEBXX_HTML_ATTR_ALIAS(hx-on-htmx-after-request, hxOnAfterRequest);
// clang-format on
// @formatter:on
}
// NOLINTEND(*-avoid-c-arrays, *-pro-bounds-array-to-pointer-decay, *-no-array-decay)
