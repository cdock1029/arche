#include "htmx.h"

constexpr QLatin1StringView getSwap(htmx::Swap swap)
{
    using namespace Qt::StringLiterals;
    using namespace htmx;
    switch (swap) {
    case Swap::InnerHtml:
        return "innerHTML"_L1;
    case Swap::OuterHtml:
        return "outerHTML"_L1;
    case Swap::BeforeBegin:
        return "beforebegin"_L1;
    case Swap::AfterBegin:
        return "afterbegin"_L1;
    case Swap::BeforeEnd:
        return "beforeend"_L1;
    case Swap::AfterEnd:
        return "afterend"_L1;
    case Swap::Delete:
        return "delete"_L1;
    case Swap::None:
        return "none"_L1;
    }
    return {};
}

namespace htmx {
using namespace Webxx::internal;
using namespace Qt::StringLiterals;

// NOLINTBEGIN(*-avoid-c-arrays, *-pro-bounds-array-to-pointer-decay, *-no-array-decay)
QHttpServerResponse&& hxRedirect(QHttpServerResponse&& resp, QByteArray&& path)
{
    resp.addHeader("HX-Redirect", std::move(path));
    return std::move(resp);
}

bool isHxRequest(const QHttpServerRequest& req)
{
    auto pair = QPair<QByteArray, QByteArray> { "HX-Request"_ba, "true"_ba };
    return req.headers().contains(pair);
}

_hxSwap::_hxSwap(Swap swap)
    : Webxx::internal::HtmlAttribute { "hx-swap", { getSwap(swap).data() } }
{
}

_hxBoost::_hxBoost(bool status)
    : Webxx::internal::HtmlAttribute { "hx-boost", { status ? "true" : "false" } }
{
}

_hxOn::_hxOn(QLatin1StringView event, QLatin1StringView js)
    : Webxx::internal::HtmlAttribute { "hx-on::%1"_L1.arg(event).toLatin1(), { js.latin1() } }
{
}

_hxOnAfterRequest::_hxOnAfterRequest(QLatin1StringView js)
    : Webxx::internal::HtmlAttribute { "hx-on::after-request", { js.latin1() } }
{
}

_hxResetAfterSuccess::_hxResetAfterSuccess()
    : Webxx::internal::HtmlAttribute { "hx-on::after-request", { "if(event.detail.successful) this.reset()" } }
{
}

_hxOnAfterSwap::_hxOnAfterSwap(QLatin1StringView js)
    : Webxx::internal::HtmlAttribute { "hx-on::after-swap", { js.latin1() } }
{
}

// NOLINTEND(*-avoid-c-arrays, *-pro-bounds-array-to-pointer-decay, *-no-array-decay)
}
